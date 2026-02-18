//------------------------------------------------------------------------------
// toggle.c
//
// Toggle the LED by pressing the button
//
// Requirements:
// - Connect an LED and a Resistor to pin PB0
// - Connect a Push Button to pin PB1 and a Pull-up Resistor
//------------------------------------------------------------------------------

#include "gpio.h"

led_t led;
button_t button;

void setup(void)
{
    button = BUTTON_new(PIN_PB1, BUTTON_ONPRESS);
    led = LED_new(PIN_PB0);
    LED_off(led);
}

void loop(void)
{
    if (BUTTON_is_active(&button))
    {
        LED_toggle(led);
    }
}
