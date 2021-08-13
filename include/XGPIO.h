#ifndef SRC_XGPIO_H_
#define SRC_XGPIO_H_

#include <stdint.h>
#define LED_Pin GPIO13
#define LED_GPIO_Port GPIOC
#define XX_Pin GPIO2
#define XX_GPIO_Port GPIOA
#define EJ_Pin GPIO3
#define EJ_GPIO_Port GPIOA
#define SS_Pin GPIO4
#define SS_GPIO_Port GPIOA
#define KIOSK_Pin GPIO1
#define KIOSK_GPIO_Port GPIOA

#define JTAG_GPIO_PORT GPIOA
#define TCK GPIO10
#define TCK_PIN 10
#define TMS GPIO0
#define TMS_PIN 0
#define TDI GPIO8
#define TDI_PIN 8
#define TDO GPIO9
#define TDO_PIN 9

void ConfigureXGPIO(void);
void SSSet(void);
void SSReset(void);
void XXSet(void);
void XXReset(void);
void EJSet(void);
void EJReset(void);
void LEDSet(void);
void LEDReset(void);
void KIOSKSet(void);
void KIOSKReset(void);

#endif 