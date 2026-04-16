//------------------------------------------------------------------------------
// analog.c
//
// Display the value of the potentiomenter
//
// Requirements:
// - Connect output pin of the potentiometer to the pin ADC0
// - Use `screen` to read value (0 to 1023) and voltage (0 to 5)
//------------------------------------------------------------------------------

#include "gpio.h"
#include "serial.h"

analog_t analog;

void setup(void)
{
	SERIAL_init();
	analog = ANALOG_new(ADC_CH4);
}

void loop(void)
{
	unsigned int res = ANALOG_read(analog);
	SERIAL_print(uint, res);
	SERIAL_print(str, ", V = ");
	SERIAL_println(uint, (5 * res) / 1023);
	delay(1000);
}
