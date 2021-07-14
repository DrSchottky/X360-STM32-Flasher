/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/st_usbfs.h>
#include "XGPIO.h"
#include "XSPI.h"
#include "XNAND.h"
#include "Delay.h"
#include "XPower.h"

#define CMD_DATA_READ   0x01
#define CMD_DATA_WRITE  0x02
#define CMD_DATA_INIT   0x03
#define CMD_DATA_DEINIT 0x04
#define CMD_DATA_STATUS 0x05
#define CMD_DATA_ERASE  0x06
#define CMD_DATA_EXEC   0x07
#define CMD_DEV_VERSION 0x08
#define CMD_XSVF_EXEC   0x09
#define CMD_POST_GET	0x0B
#define CMD_XBOX_PWRON  0x10
#define CMD_XBOX_PWROFF 0x11
#define CMD_DEV_UPDATE  0xF0

void TX_ReadData(uint8_t len, uint8_t* buffer);
void RX_WriteFlash(uint8_t len, uint8_t* buffer);

static struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_VENDOR,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0xffff,
	.idProduct = 0x0004,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};


static const struct usb_endpoint_descriptor data_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x05,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
}};


static const struct usb_interface_descriptor data_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_DATA,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = data_endp,
}};

static const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = data_iface,
}};

static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,

	.interface = ifaces,
};

static const char *usb_strings[] = {
	"Black Sphere Technologies",
	"DrSchottky Flasher",
	"DEMO",
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];
uint32_t Status = 0;
uint32_t wordsLeft;
uint32_t nextBlock;
uint32_t TX_ToSend = 0;
uint32_t RX_ToReceive  = 0;
uint8_t TX_Buffer[64] = {0};
uint8_t RX_Buffer[64] = {0};
uint8_t bytesToSend;
uint8_t bytesToReceive;
uint8_t commandProcess;
uint8_t post_code = 0;
volatile uint8_t last_read_post_code = 0;
bool is_jrp = false;

void TX_ReadData(uint8_t len, uint8_t* buffer)
{
    len /= 4;

    while(len)
    {
        uint8_t readNow;

        if(!wordsLeft)
        {
            Status = XNAND_StartRead(nextBlock);
            nextBlock++;
            wordsLeft = 0x84;
        }

        readNow = (len < wordsLeft) ? len : wordsLeft;
        XNAND_ReadFillBuffer(buffer, readNow);

        buffer += (readNow*4);
        wordsLeft -= readNow;
        len -= readNow;
    }
}

void RX_WriteFlash(uint8_t len, uint8_t* buffer)
{
    len /= 4;

    if(commandProcess == 0)
    {
        Status = XNAND_Erase(nextBlock);
        XNAND_StartWrite();
        commandProcess = 1;
    }

    while(len)
    {
        uint8_t writeNow;

        if(!wordsLeft)
        {
            nextBlock++;
            wordsLeft = 0x84;
        }

        writeNow = (len < wordsLeft) ? len : wordsLeft;
        XNAND_WriteProcess(buffer, writeNow);

        buffer += (writeNow*4);
        wordsLeft -= writeNow;
        len -= writeNow;

        //execute write if buffer in NAND controller is filled
        if(!wordsLeft)
        {
            Status = XNAND_WriteExecute(nextBlock-1);
            XNAND_StartWrite();
        }
    }
}


static enum usbd_request_return_codes cdcacm_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
		uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest) {
	case CMD_DEV_VERSION:
	{
		uint32_t * data = *(uint32_t **)buf;
		volatile uint32_t argA = *data;
		volatile uint32_t argB = *(data + 1);
		uint8_t version[4] = {1, 0, 0, 0};
		if(is_jrp)
		{
			version[0] = 16;
		}
		usbd_ep_write_packet(usbd_dev, 0x82, version, 4);
		return USBD_REQ_HANDLED;
	}
	case CMD_DATA_INIT:
	{
		XSPI_EnterFlashmode();
		uint8_t tx_buf [4] = {0};
		XSPI_Read(0, tx_buf);
		XSPI_Read(0, tx_buf);
		usbd_ep_write_packet(usbd_dev, 0x82, tx_buf, 4);
		return USBD_REQ_HANDLED;
	}
	case CMD_DATA_READ:
	{
		uint32_t * data = *(uint32_t **)buf;
		nextBlock = *data << 5;
		TX_ToSend = *(data + 1);
		Status = 0;
	  	wordsLeft = 0;
		bytesToSend = (TX_ToSend > sizeof(TX_Buffer)) ? sizeof(TX_Buffer) : TX_ToSend;
		if(TX_ToSend)
		{
			TX_ReadData(bytesToSend, TX_Buffer);
			usbd_ep_write_packet(usbd_dev, 0x82, TX_Buffer, bytesToSend);
			TX_ToSend -= bytesToSend;
		}
		return USBD_REQ_HANDLED;
	}
	case CMD_DATA_WRITE:
	{
		uint32_t * data = *(uint32_t **)buf;
		nextBlock = *data << 5;
		RX_ToReceive = *(data + 1);
		Status = 0;
	  	wordsLeft = 0;
		commandProcess = 0;
		bytesToReceive = (RX_ToReceive > sizeof(RX_Buffer)) ? sizeof(RX_Buffer) : RX_ToReceive;
		return USBD_REQ_HANDLED;
	}
	case CMD_DATA_STATUS:
	{
		usbd_ep_write_packet(usbd_dev, 0x82, (uint8_t *)&Status, sizeof(Status));
		return USBD_REQ_HANDLED;
	}
	case CMD_DATA_ERASE:
	{
		uint32_t * data = *(uint32_t **)buf;
		nextBlock = *data << 5;
		Status = XNAND_Erase(nextBlock);
		usbd_ep_write_packet(usbd_dev, 0x82, (uint8_t *)"\0\0\0\0", 4);
		return USBD_REQ_HANDLED;
	}
	case CMD_DATA_DEINIT:
	{
		XSPI_LeaveFlashmode();
		return USBD_REQ_HANDLED;
	}
	case CMD_XBOX_PWRON:
	{
		PowerUp();
		return USBD_REQ_HANDLED;
	}
	case CMD_XBOX_PWROFF:
	{
		Shutdown();
		return USBD_REQ_HANDLED;
	}
	case CMD_POST_GET:
	{
		uint8_t post_resp;
		if(last_read_post_code == post_code)
			post_resp = 0;
		else
			post_resp = post_code;
		usbd_ep_write_packet(usbd_dev, 0x82, (uint8_t*)&post_resp, 1);
		last_read_post_code = post_code;
		return USBD_REQ_HANDLED;
	}
	}
	return USBD_REQ_NOTSUPP;
}

static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)ep;
	(void)usbd_dev;
	if(bytesToReceive == 0)
	{
		return;
	}

	int len = usbd_ep_read_packet(usbd_dev, 0x05, RX_Buffer, bytesToReceive);

	if (len) {
			RX_WriteFlash(len, RX_Buffer);
			RX_ToReceive -= bytesToReceive;
			bytesToReceive = (RX_ToReceive > sizeof(RX_Buffer)) ? sizeof(RX_Buffer) : RX_ToReceive;
	}
}

static void cdcacm_data_tx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	while(TX_ToSend)
	{
		TX_ReadData(bytesToSend, TX_Buffer);
		usbd_ep_write_packet(usbd_dev, ep, TX_Buffer, bytesToSend);
		TX_ToSend -= bytesToSend;
	}
	return;
}

static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;
	(void)usbd_dev;

	usbd_ep_setup(usbd_dev, 0x05, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_tx_cb);

	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_VENDOR,
				USB_REQ_TYPE_TYPE,
				cdcacm_control_request);
}


int main(void)
{
	usbd_device *usbd_dev;

	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_SPI1);

	/* Setup GPIOC Pin 12 to pull up the D+ high, so autodect works
	 * with the bootloader.  The circuit is active low. */
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_OPENDRAIN, GPIO12);
	gpio_clear(GPIOC, GPIO12);
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO9 | GPIO8 | GPIO7 | GPIO6 | GPIO5 | GPIO4 | GPIO13 | GPIO14);
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO2);
	is_jrp = gpio_get(GPIOB,GPIO2) != 0;
	if(is_jrp)
	{
		/* JR-P Mode */
		dev.idVendor = 0x11d4;
		dev.idProduct = 0x8338;
	}

	usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);
	clock_setup();
	ConfigureXGPIO();
	XSPI_Setup();
	while (1)
	{
		usbd_poll(usbd_dev);
		if(gpio_get(GPIOB, GPIO9))
			post_code &= ~0x01;
		else
			post_code |= 0x01;
		if(gpio_get(GPIOB, GPIO7))
			post_code &= ~0x02;
		else
			post_code |= 0x02;
		if(gpio_get(GPIOB, GPIO5))
			post_code &= ~0x04;
		else
			post_code |= 0x04;
		if(gpio_get(GPIOB, GPIO13))
			post_code &= ~0x08;
		else
			post_code |= 0x08;
		if(gpio_get(GPIOB, GPIO8))
			post_code &= ~0x10;
		else
			post_code |= 0x10;
		if(gpio_get(GPIOB, GPIO6))
			post_code &= ~0x20;
		else
			post_code |= 0x20;
		if(gpio_get(GPIOB, GPIO4))
			post_code &= ~0x40;
		else
			post_code |= 0x40;
		if(gpio_get(GPIOB, GPIO14))
			post_code &= ~0x80;
		else
			post_code |= 0x80;
	}
}
