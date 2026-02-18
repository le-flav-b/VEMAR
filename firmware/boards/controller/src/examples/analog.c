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

analog_t pot;

void setup(void)
{
	SERIAL_init();
	ADC_init();
	pot = ANALOG_new(PIN_PC0);
}

void loop(void)
{
	unsigned int analog = ADC_read(ADC_CHANNEL_0);
	SERIAL_print(analog, uint);
	SERIAL_print(", V = ", str);
	SERIAL_println((5 * analog) / 1023, uint);
	delay(1000);
}
