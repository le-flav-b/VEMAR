//------------------------------------------------------------------------------
// blink.c
//
// Make LED blink
//
// Requirements:
// - Connect an LED and a Resistor to pin PB5
//------------------------------------------------------------------------------

#include "led.h"

led_t led;

void setup(void)
{
	led = LED_new(PIN_PB5);
}

void loop(void)
{
	LED_on(led);
	delay(1000);
	LED_off(led);
	delay(1000);
}
