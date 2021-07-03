#include "XGPIO.h"
#include <libopencm3/stm32/gpio.h>

void ConfigureXGPIO()
{
    LEDSet();
    XXSet();
    EJSet();
    SSSet();
    gpio_set_mode(LED_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, LED_Pin);
    gpio_set_mode(XX_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, XX_Pin);
    gpio_set_mode(EJ_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, EJ_Pin);
    gpio_set_mode(SS_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, SS_Pin);
}
void SSSet()
{
    gpio_set(SS_GPIO_Port, SS_Pin);
}
void SSReset()
{
    gpio_clear(SS_GPIO_Port, SS_Pin);
}
void XXSet()
{
    gpio_set(XX_GPIO_Port, XX_Pin);
}
void XXReset()
{
    gpio_clear(XX_GPIO_Port, XX_Pin);
}
void EJSet()
{
    gpio_set(EJ_GPIO_Port, EJ_Pin);
}
void EJReset()
{
    gpio_clear(EJ_GPIO_Port, EJ_Pin);
}
void LEDSet()
{
    gpio_set(LED_GPIO_Port, LED_Pin);
}
void LEDReset()
{
    gpio_clear(LED_GPIO_Port, LED_Pin);
}