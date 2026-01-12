#include <avr/io.h>

#include "joystick.h"
#include "adc.h"

void JOYSTICK_init(byte_t x, byte_t y, byte_t btn)
{
    BIT_clear(DDRC, BIT(x)); // set analog pin x as input
    BIT_clear(DDRC, BIT(y)); // set analog pin y as input

    BIT_clear(DDRD, BIT(btn)); // set button as input
    BIT_set(PORTD, BIT(btn));  // enable pull-up resistor

    ADC_enable_channel(x);
    ADC_enable_channel(y);
}

void JOYSTICK_read(struct joystick *joy, byte_t x, byte_t y, byte_t btn)
{
    joy->x = ADC_read(x);
    joy->y = ADC_read(y);
    (joy->button).pressed = (BIT_read(PIND, BIT(btn)) == 0);
}
