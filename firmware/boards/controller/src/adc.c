#include <avr/interrupt.h>

#include "adc.h"

typedef void (*adc_callback_t)(uint16_t);

static volatile adc_callback_t g_adc_callback;

//------------------------------------------------------------------------------
// ADC_init
//------------------------------------------------------------------------------
void ADC_init(adc_ref_t reference, adc_bit_t resolution, adc_ps_t prescaler)
{
    if (!ADC_is_enabled())
    {
        ADMUX = reference | resolution; // set voltage referecne and resolution
        ADCSRA = BIT(ADEN) | prescaler; // enable ADC and set prescaler
    } // if not initialized
}

//------------------------------------------------------------------------------
// ADC_reset
//------------------------------------------------------------------------------
void ADC_reset(void)
{
    ADMUX = 0x00;  // ADC Multiplexer Selection Register
    ADCSRA = 0x00; // ADC Control and Status Register A
    ADCSRB = 0x00; // ADC Control and Status Register B
    DIDR0 = 0x00;  // Digital Input Disable Register
}

//------------------------------------------------------------------------------
// ADC_enable_channel
//------------------------------------------------------------------------------
void ADC_enable_channel(adc_ch_t channel)
{
    if (ADC_CH5 >= channel)
    {
        BIT_clear(DDRC, BIT(channel));  // set as input
        BIT_clear(PORTC, BIT(channel)); // disable internal Pull-up Resistor
        BIT_set(DIDR0, BIT(channel));   // disable Digital Input Buffer
    } // if channel 0 to 5
}

//------------------------------------------------------------------------------
// ADC_data
//------------------------------------------------------------------------------
uint16_t ADC_data(void)
{
    uint16_t result;

    if (BIT_is_set(ADMUX, BIT(ADLAR)))
    {
        result = ADCH;
    } // if left adjusted (8-bit resolution)
    else
    {
        result = ADCL; // need to be read first
        result = result | (ADCH << 8);
    } // if right adjusted (10-bit resolution)
    return (result);
}

//------------------------------------------------------------------------------
// ADC_read
//------------------------------------------------------------------------------
uint16_t ADC_read(adc_ch_t channel)
{
    ADC_set_channel(channel);
    ADC_start();
    WAIT_UNTIL(ADC_is_complete());
    return (ADC_data());
}

//------------------------------------------------------------------------------
// ADC_attach_interrupt
//------------------------------------------------------------------------------
void ADC_attach_interrupt(void (*on_complete)(uint16_t))
{
    g_adc_callback = on_complete;
}

//------------------------------------------------------------------------------
// ISR
//------------------------------------------------------------------------------
ISR(ADC_vect)
{
    if (NULL != g_adc_callback)
    {
        uint16_t res = ADC_data();
        g_adc_callback(res);
    }
}

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
extern inline bool_t ADC_is_enabled(void);
extern inline bool_t ADC_is_complete(void);

extern inline void ADC_enable(void);
extern inline void ADC_enable_channel(adc_ch_t);
extern inline void ADC_enable_autotrigger(void);
extern inline void ADC_enable_interrupt(void);

extern inline void ADC_disable(void);
extern inline void ADC_disable_channel(adc_ch_t);
extern inline void ADC_disable_autotrigger(void);
extern inline void ADC_disable_interrupt(void);

extern inline adc_ref_t ADC_get_reference(void);
extern inline adc_bit_t ADC_get_resolution(void);
extern inline adc_ps_t ADC_get_prescaler(void);
extern inline adc_ch_t ADC_get_channel(void);

extern inline void ADC_set_reference(adc_ref_t);
extern inline void ADC_set_resolution(adc_bit_t);
extern inline void ADC_set_prescaler(adc_ps_t);
extern inline void ADC_set_channel(adc_ch_t);

extern inline void ADC_start(void);
