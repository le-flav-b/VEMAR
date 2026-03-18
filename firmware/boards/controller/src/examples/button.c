//------------------------------------------------------------------------------
// button.c
//
// Turn LED on by pressing the button, and turn LED off by releasing it
//
// Requirements:
// - Connect an LED and a Resistor to pin PB0
// - Connect a Push Button to pin PB1 and a Pull-up Resistor
//------------------------------------------------------------------------------

#include "gpio.h"

led_t led;
button_t btn_down;
button_t btn_up;

void setup(void)
{
    btn_down = BUTTON_new(PIN_PD3, BUTTON_ONPRESS);
    btn_up = BUTTON_new(PIN_PD4, BUTTON_ONHOLD);
    led = LED_new(PIN_PD5);
    LED_off(led);
}

void loop(void)
{
    if (BUTTON_is_active(&btn_down) || BUTTON_is_active(&btn_up))
    {
        LED_on(led);
    }
    else 
    {
        LED_off(led);
    }
}
