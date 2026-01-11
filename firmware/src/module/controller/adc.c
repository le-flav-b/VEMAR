#include "adc.h"

void ADC_reset(void)
{
	ADMUX = 0x00; // ADC Multiplexer Selection Register
	ADCSRA = 0x00; // ADC Control and Status Register A
	ADCSRB = 0x00; // ADC Control and Status Register B
	DIDR0 = 0x3F; // Digital Input Disable Register
}

void ADC_init(byte_t reference, byte_t prescaler, byte_t result)
{
	ADC_reset();
	ADC_enable();
	ADC_set_reference(reference);
	ADC_set_prescaler(prescaler);
	ADC_set_result(result);

	// BIT_set(ADCSRA, BIT(ADEN));                       // enable ADC
	// BIT_write(ADMUX, reference, ADC_MASK_REFERENCE); // write bits [7:6]
	// BIT_write(ADCSRA, prescaler, ADC_MASK_PRESCALER); // write bits [2:0]
	// BIT_write(ADMUX, result, ADC_MASK_RESULT);        // Write bit 5
}

unsigned int ADC_read(byte_t channel)
{
	ADC_set_channel(channel);
	ADC_start_conversion();
	WAIT_UNTIL(0 == BIT_read(ADCSRA, BIT(ADSC)));

    // BIT_write(ADMUX, channel, ADC_MASK_CHANNEL); // select channel
    // BIT_set(ADCSRA, BIT(ADSC));                 // start ADC conversion
    // WAIT_UNTIL(!BIT_read(ADCSRA, BIT(ADSC)));    // wait conversion completion
    return ADC;
}
