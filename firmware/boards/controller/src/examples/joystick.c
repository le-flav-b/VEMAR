//------------------------------------------------------------------------------
// joystick.c
//
// Read the values from the joystick
//
// Requirements:
// - Connect VRx pin of Joystick to ADC0 (PC0)
// - Connect VRy pin of Joystick to ADC1 (PC1)
// - Connect SW pin of Joystick to PC2
// - Use `screen` program to read value
//------------------------------------------------------------------------------

#include "joystick.h"
#include "serial.h"

joystick_t joy;

void setup(void)
{
    SERIAL_init();
    ADC_init();
    joy = JOYSTICK_new(PIN_PC0, PIN_PC1, PIN_PC2);
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
