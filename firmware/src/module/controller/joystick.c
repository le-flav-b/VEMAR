#include <avr/io.h>

#include "joystick.h"

void JOYSTICK_init(byte_t x, byte_t y, byte_t btn)
{
    BIT_clear(DDRC, BIT(x)); // set x as input
    BIT_clear(DDRC, BIT(y)); // set y as input

    BIT_clear(DDRD, BIT(btn)); // set button as input
    BIT_set(PORTD, BIT(btn));  // enable pull-up resistor
}

void JOYSTICK_read(struct joystick *joy, byte_t x, byte_t y, byte_t btn)
{
    joy->x = ADC_read(x);
    joy->y = ADC_read(y);
    joy->button = (BIT_read(PIND, BIT(btn)) == 0);
}
