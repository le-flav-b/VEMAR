//------------------------------------------------------------------------------
// joystick.c
//
// Read the values from the joystick
//
// Requirements:
// - Connect VRx pin of Joystick to ADC7
// - Connect VRy pin of Joystick to ADC6
// - Connect SW pin of Joystick to PC5
// - Use `screen` program to read value
//------------------------------------------------------------------------------

#include "joystick.h"
#include "serial.h"

joystick_t joy;

void setup(void)
{
    SERIAL_init();
    joy = JOYSTICK_new(ADC_CH7, ADC_CH6, PIN_PC5);
}

void loop()
{
    SERIAL_print(str, "(X, Y) = (");
    SERIAL_print(uint, JOYSTICK_x(&joy));
    SERIAL_print(str, ", ");
    SERIAL_print(uint, JOYSTICK_y(&joy));
    if (JOYSTICK_is_pressed(&joy))
    {
        SERIAL_println(str, "), PRESSED");
    }
    else
    {
        SERIAL_println(str, "), released");
    }
    delay(1000);
}
