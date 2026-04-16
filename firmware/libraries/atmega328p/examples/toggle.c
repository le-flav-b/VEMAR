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
#include "serial.h"

button_t btn_down;
button_t btn_up;

void setup(void)
{
    btn_up = BUTTON_new(PIN_PC3, BUTTON_ONHOLD);
    btn_down = BUTTON_new(PIN_PC2, BUTTON_ONHOLD);
    SERIAL_init();
}

void loop(void)
{
    if (BUTTON_is_active(&btn_up))
    {
        SERIAL_println(str, "button UP");
    }
    else if (BUTTON_is_active(&btn_down))
    {
        SERIAL_println(str, "button DOWN");
    }
    else
    {
        SERIAL_println(str, "OFF");
    }
    delay(1000);
}
