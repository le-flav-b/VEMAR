#include "adc.h"

void ADC_reset(void)
{
	ADMUX = 0x00; // ADC Multiplexer Selection Register
	ADCSRA = 0x00; // ADC Control and Status Register A
	ADCSRB = 0x00; // ADC Control and Status Register B
	// DIDR0 = 0x3F; // Digital Input Disable Register
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

inline unsigned int ADC_data(void)
{
    unsigned int result;
    if (BIT_read(ADMUX, BIT(ADLAR)))
    {
        result = ADCH;
    } // if left adjusted (ADLAR = 1)
    else {
        result = ADCL;
        result = result | (ADCH << 8);
    } // if right adjusted
    return result;
}


unsigned int ADC_read(byte_t channel)
{
	ADC_set_channel(channel);
	ADC_start_conversion();
	WAIT_UNTIL(ADC_is_conversion_complete());
    return ADC_data();
}

extern inline void ADC_set_reference(byte_t);
extern inline void ADC_set_result(byte_t);
extern inline void ADC_set_channel(byte_t);

extern inline void ADC_enable(void);
extern inline void ADC_disable(void);

extern inline void ADC_start_conversion(void);
extern inline void ADC_enable_autotrigger(void);
extern inline void ADC_disable_autotrigger(void);

extern inline bool_t ADC_is_conversion_complete(void);
extern inline void ADC_enable_interrupt(void);
extern inline void ADC_disable_interrupt(void);

extern inline void ADC_set_prescaler(byte_t);

extern inline void ADC_enable_channel(byte_t);
extern inline void ADC_disable_channel(byte_t);
