#include <avr/io.h>

#include "adc.h"

#define ADC_MASK_REFERENCE 0xC0
#define ADC_MASK_PRESCALER 0x07
#define ADC_MASK_RESULT 0x20
#define ADC_MASK_CHANNEL 0x0F

void ADC_init(byte_t reference, byte_t prescaler, byte_t result)
{
    BIT_set(ADCSRA, BIT(ADEN));                       // enable ADC
    BIT_write(ADCSRA, reference, ADC_MASK_REFERENCE); // write bits [7:6]
    BIT_write(ADCSRA, prescaler, ADC_MASK_PRESCALER); // write bits [2:0]
    BIT_write(ADMUX, result, ADC_MASK_RESULT);        // Write bit 5
}

unsigned int ADC_read(byte_t channel)
{
    BIT_set(ADMUX, ADC_MASK_CHANNEL & channel); // select channel
    BIT_set(ADCSRA, BIT(ADSC));                 // start ADC conversion
    WAIT_UNTIL(BIT_read(ADCSRA, BIT(ADIF)));    // wait conversion completion
    return ADC;
}
