#ifndef SRC_XNAND_H_
#define SRC_XNAND_H_
#include <stdint.h>

void XNAND_ClearStatus(void);
uint16_t XNAND_GetStatus(void);
uint8_t XNAND_WaitReady(uint16_t timeout);
uint16_t XNAND_StartRead(uint32_t blockNum);
void XNAND_ReadFillBuffer(uint8_t* buf, uint8_t wordCount);
uint16_t XNAND_Erase(uint32_t blockNum);
void XNAND_StartWrite(void);
void XNAND_WriteProcess(uint8_t* buf, uint8_t wordCount);
uint16_t XNAND_WriteExecute(uint32_t blockNum);

#endif