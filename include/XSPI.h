/*
 * XSPI.h
 *
 *  Created on: May 6, 2021
 *      Author: User
 */

#ifndef SRC_XSPI_H_
#define SRC_XSPI_H_

#include <stdint.h>

void XSPI_Setup(void);
void SPI1_Transfer(uint8_t *, uint8_t *, int);
void XSPI_EnterFlashmode(void);
void XSPI_Read(uint8_t, uint8_t*);
uint16_t XSPI_ReadWord(uint8_t);
uint8_t XSPI_ReadByte(uint8_t);
void XSPI_Write(uint8_t, uint8_t*);
void XSPI_WriteDword(uint8_t, uint32_t);
void XSPI_WriteByte(uint8_t, uint8_t);
void XSPI_Write0(uint8_t);

#endif /* SRC_XSPI_H_ */
