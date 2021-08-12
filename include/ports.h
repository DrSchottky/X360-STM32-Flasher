/*******************************************************/
/* file: ports.h                                       */
/* abstract:  This file contains extern declarations   */
/*            for providing stimulus to the JTAG ports.*/
/*******************************************************/

#ifndef ports_dot_h
#define ports_dot_h
#include <stdint.h>
#include <libopencm3/stm32/gpio.h>

/* these constants are used to send the appropriate ports to setPort */
/* they should be enumerated types, but some of the microcontroller  */
/* compilers don't like enumerated types */

/*
 Pin assignment on the bluepill:
 TCK - PB6
 TMS - PB7
 TDI - PB8
 TDO - PB9
 */

#define JTAG_GPIO_PORT GPIOB
#define TCK GPIO6
#define TCK_PIN 6
#define TMS GPIO7
#define TMS_PIN 7
#define TDI GPIO8
#define TDI_PIN 8
#define TDO GPIO5
#define TDO_PIN 5

extern uint8_t xsvf_read_data(void);
extern void u_sleep( uint32_t m );

/* Sets up the ports ready */
extern void initPorts(void);

/* set the port "p" (TCK, TMS, or TDI) to val (0 or 1) */
extern void setPort(uint16_t p, uint8_t val);

/* read the TDO bit and store it in val */
inline int readTDOBit(void)
{
  // return digitalRead(TDO);
  return (GPIOB_IDR & (1u << TDO_PIN)) != 0;
}

/* make clock go down->up->down*/
extern void pulseClock(void);

/* read the next byte of data from the xsvf file */
extern void readByte(uint8_t *data);

extern void waitTime(uint32_t microsec);

#endif
