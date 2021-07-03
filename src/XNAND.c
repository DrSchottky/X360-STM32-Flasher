#include "XNAND.h"
#include "XSPI.h"
#include "XGPIO.h"

void XNAND_ClearStatus(void)
{
    uint8_t tmp[4] = {0};
    XSPI_Read(0x04, tmp);
    XSPI_Write(0x04, tmp);
}
uint16_t XNAND_GetStatus(void)
{
    return XSPI_ReadWord(0x04);
}
uint8_t XNAND_WaitReady(uint16_t timeout)
{
	do {
		if (!(XSPI_ReadByte(0x04) & 0x01))
			return 1;
	} while (timeout--);

	return 0;
}
uint16_t XNAND_StartRead(uint32_t blockNum)
{
    uint16_t res;

    XNAND_ClearStatus();

    XSPI_WriteDword(0x0C, blockNum << 9);

    XSPI_WriteByte(0x08, 0x03);

    if (!XNAND_WaitReady(0x1000))
		return 0x8011;

	res = XNAND_GetStatus();

    XSPI_Write0(0x0C);

    return res;
}

void XNAND_ReadFillBuffer(uint8_t* buf, uint8_t wordCount)
{

	uint8_t data_first [5] = {(0x08 << 2) | 2, 0,0,0,0};
	uint8_t data_second [6] = {(0x10 << 2) | 1, 0xFF, 0, 0, 0, 0};
	uint8_t outbuf[6];
    while(wordCount--)
    {

		SSReset();
		SPI1_Transfer(data_first, outbuf, sizeof(data_first));
		SSSet();
		SSReset();
		SPI1_Transfer(data_second, outbuf, sizeof(data_second));
		*(uint32_t *)buf = *((uint32_t *)&outbuf[2]);
		SSSet();
        buf += 4;
    }
}

uint16_t XNAND_Erase(uint32_t blockNum)
{
	uint8_t tmp[4];

	XNAND_ClearStatus();

	XSPI_Read(0, tmp);
	tmp[0] |= 0x08;
	XSPI_Write(0, tmp);

	XSPI_WriteDword(0x0C, blockNum << 9);

	if (!XNAND_WaitReady(0x1000))
		return 0x8001;

	XSPI_WriteByte(0x08, 0xAA);
	XSPI_WriteByte(0x08, 0x55);

	if (!XNAND_WaitReady(0x1000))
		return 0x8002;

	XSPI_WriteByte(0x08, 0x5);

	if (!XNAND_WaitReady(0x1000))
		return 0x8003;

	return XNAND_GetStatus();
}

void XNAND_StartWrite()
{
    XNAND_ClearStatus();
    XSPI_Write0(0x0C);
}

void XNAND_WriteProcess(uint8_t* buf, uint8_t wordCount)
{
    while(wordCount--)
    {
        XSPI_Write(0x10, buf);
        XSPI_WriteByte(0x08, 0x01);
        buf += 4;
    }
}

uint16_t XNAND_WriteExecute(uint32_t blockNum)
{
    XSPI_WriteDword(0x0C, blockNum << 9);

    XSPI_WriteByte(0x08, 0x55);
	XSPI_WriteByte(0x08, 0xAA);
	XSPI_WriteByte(0x08, 0x4);

    if (!XNAND_WaitReady(0x1000))
		return 0x8021;

    return XNAND_GetStatus();
}
