#include "XSPI.h"
#include "XGPIO.h"
#include "Delay.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/spi.h>

void XSPI_Setup(void)
{
	SSSet();
	XXSet();
	EJSet();

	/* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
											GPIO5 |
											GPIO7 );

	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
			GPIO6);

	/* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
	spi_reset(SPI1);

	/* Set up SPI in Master mode with:
	* Clock baud rate: 1/64 of peripheral clock frequency
	* Clock polarity: Idle High
	* Clock phase: Data valid on 2nd clock pulse
	* Data frame format: 8-bit
	* Frame format: MSB First
	*/
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_4, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
					SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_LSBFIRST);

	/*
	* Set NSS management to software.
	*
	* Note:
	* Setting nss high is very important, even if we are controlling the GPIO
	* ourselves this bit needs to be at least set to 1, otherwise the spi
	* peripheral will not send any data out.
	*/
	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);

	/* Enable SPI1 periph. */
	spi_enable(SPI1);
	}

void SPI1_Transfer(uint8_t *outp, uint8_t *inp, int count) {
    while(count--) {
        while(!(SPI1_SR & SPI_SR_TXE))
            ;
        *(volatile uint8_t *)&SPI1_DR = *outp++;
        while(!(SPI1_SR & SPI_SR_RXNE))
            ;
        *inp++ = *(volatile uint8_t *)&SPI1_DR;
    }
}

void XSPI_Powerup(void)
{
	KIOSKReset();
	msleep(22);
	KIOSKSet();
	msleep(22);
	
	KIOSKReset();
	msleep(22);
	KIOSKSet();
	msleep(22);
	
	KIOSKReset();
	msleep(22);
	KIOSKSet();
	msleep(22);
	
	KIOSKReset();
	msleep(22);
	KIOSKSet();
}

void XSPI_Shutdown(void)
{
	SSSet();
	XXReset();
	EJReset();
	
	msleep(50);
	
	EJSet();
}

void XSPI_EnterFlashmode(void)

{
	XXReset();

	msleep(50);

	SSReset();
	EJReset();

	msleep(50);

	XXSet();
	EJSet();

	msleep(50);
}

void XSPI_LeaveFlashmode(void)
{
	SSSet();
	EJReset();
	
	msleep(50);
	
	XXReset();
	EJSet();
}

void XSPI_Read(uint8_t reg, uint8_t* buf)

{
	uint8_t data[6] = {(reg << 2) | 1, 0xFF, 0, 0, 0, 0};
	SSReset();
	SPI1_Transfer(data, data, sizeof(data));
	*(uint32_t*)buf = *((uint32_t*)&data[2]);
	SSSet();
}
uint16_t XSPI_ReadWord(uint8_t reg)
{
	uint16_t res;
	uint8_t data[4] = {(reg << 2) | 1, 0xFF, 0, 0};
	SSReset();
	SPI1_Transfer(data, data, sizeof(data));
	res = *((uint16_t*)&data[2]);
	SSSet();
	return res;
}
uint8_t XSPI_ReadByte(uint8_t reg)
{
	uint8_t res;
	uint8_t data[3] = {(reg << 2) | 1, 0xFF, 0};
	SSReset();
	SPI1_Transfer(data, data, sizeof(data));
	res = data[2];
	SSSet();
	return res;

}
void XSPI_Write(uint8_t reg, uint8_t* buf)

{
	uint8_t data[5] = {(reg << 2) | 2, 0, 0, 0, 0};
	*((uint32_t*)&data[1]) = *(uint32_t*)buf;
	SSReset();
	SPI1_Transfer(data, data, sizeof(data));
	SSSet();
}
void XSPI_WriteDword(uint8_t reg, uint32_t dword)
{
    XSPI_Write(reg, (uint8_t*)&dword);
}
void XSPI_WriteByte(uint8_t reg, uint8_t byte)
{
    uint8_t data[] = {0,0,0,0};
    data[0] = byte;

    XSPI_Write(reg, data);
}
void XSPI_Write0(uint8_t reg)
{
    uint8_t tmp[] = {0,0,0,0};
    XSPI_Write(reg, tmp);
}
