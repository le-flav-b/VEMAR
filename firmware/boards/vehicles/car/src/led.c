#include "led.h"

void led_init(void)
{
	GPIO_OUTPUT(LED_GREEN); led_green_off();
	GPIO_OUTPUT(LED_RED); led_red_off();
}

void led_green_on(void) { GPIO_SET(LED_GREEN); }
void led_green_off(void) { GPIO_CLEAR(LED_GREEN); }
void led_green_toggle(void) { GPIO_TOGGLE(LED_GREEN); }
void led_red_on(void) { GPIO_SET(LED_RED); }
void led_red_off(void) { GPIO_CLEAR(LED_RED); }
void led_red_toggle(void) { GPIO_TOGGLE(LED_RED); }
