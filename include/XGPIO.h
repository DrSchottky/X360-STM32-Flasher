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

void ConfigureXGPIO(void);
void SSSet(void);
void SSReset(void);
void XXSet(void);
void XXReset(void);
void EJSet(void);
void EJReset(void);
void LEDSet(void);
void LEDReset(void);

#endif 