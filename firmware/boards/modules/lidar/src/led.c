#include "led.h"

void led_init(void)
{
	SET_MSK(LEDS_DDR, MSK(LED_GREEN_PIN) | MSK(LED_RED_PIN));
	CLEAR_MSK(LEDS_PORT, MSK(LED_GREEN_PIN) | MSK(LED_RED_PIN));
}

void led_green_on(void) { SET(LEDS_PORT, LED_GREEN_PIN); }
void led_green_off(void) { CLEAR(LEDS_PORT, LED_GREEN_PIN); }
void led_green_toggle(void) { TOGGLE(LEDS_PORT, LED_GREEN_PIN); }
void led_red_on(void) { SET(LEDS_PORT, LED_RED_PIN); }
void led_red_off(void) { CLEAR(LEDS_PORT, LED_RED_PIN); }
void led_red_toggle(void) { TOGGLE(LEDS_PORT, LED_RED_PIN); }
