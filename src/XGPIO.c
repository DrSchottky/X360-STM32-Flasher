#include "XGPIO.h"
#include <libopencm3/stm32/gpio.h>

void ConfigureXGPIO()
{
    LEDSet();
    XXSet();
    EJSet();
    SSSet();
    KIOSKSet();
    gpio_set_mode(LED_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, LED_Pin);
    gpio_set_mode(XX_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, XX_Pin);
    gpio_set_mode(EJ_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, EJ_Pin);
    gpio_set_mode(SS_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, SS_Pin);
    gpio_set_mode(KIOSK_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, KIOSK_Pin);
    
    gpio_set_mode(JTAG_GPIO_PORT, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, TCK);
    gpio_set_mode(JTAG_GPIO_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, TMS);
    gpio_set_mode(JTAG_GPIO_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, TDI);
    gpio_set_mode(JTAG_GPIO_PORT, GPIO_MODE_INPUT,
                GPIO_CNF_INPUT_PULL_UPDOWN, TDO);
    gpio_set(JTAG_GPIO_PORT, TDO);
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
void KIOSKSet()
{
    gpio_set(KIOSK_GPIO_Port, KIOSK_Pin);
}
void KIOSKReset()
{
    gpio_clear(KIOSK_GPIO_Port, KIOSK_Pin);
}