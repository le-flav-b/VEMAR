#ifndef VEMAR_ADC_H
#define VEMAR_ADC_H

#include "common.h"

#if !defined(_DOXYGEN_)
// Masks used by ADC, for internal use only
#define _ADC_MASK_REFERENCE 0xC0 /**< ADMUX[7:6] */
#define _ADC_MASK_PRESCALER 0x07 /**< ADCSRA[2:0] */
#define _ADC_MASK_RESULT 0x20    /**< ADMUX[5] */
#define _ADC_MASK_CHANNEL 0x0F   /**< ADMUX[3:0] */
#define _ADC_MASK_INPUT 0x3F     /**< DIDR0[5:0] */
#endif

#define ADC_VALUE_MAX 0x3F /**< Maximum value of ADC 10-bit result */

//------------------------------------------------------------------------------
// Enumerations
//------------------------------------------------------------------------------

/**
 * @brief Define the Analog Voltage Reference for the __ADC__
 * @details
 * This enumeration maps to the __REFS[1:0]__ bits of the __ADMUX__ register
 * @see ADC_init
 * @see ADC_get_reference
 * @see ADC_set_reference
 */
typedef enum
{
    ADC_AREF = 0x00,  /**< __AREF__, internal VREF turned off */
    ADC_AVCC = 0x40,  /**< __AVCC__ with external capacitor at AREF pin */
    ADC_INTERN = 0xC0 /**< Internal 1.1V voltage with external capacitor */
} adc_ref_t;

/**
 * @brief Define the presentation for the ADC conversion result
 * @details
 * This enumeration maps to the __ADLAR__ bit of the __ADMUX__ register
 * @see ADC_init
 * @see ADC_get_resolution
 * @see ADC_set_resolution
 */
typedef enum
{
    ADC_10BIT = 0x00, /**< 10-bit result (Right Adjusted) */
    ADC_8BIT = 0x20   /**< 8-bit result (Left Adjusted) */
} adc_bit_t;

/**
 * @brief Define the division factor between the system clock frequency and
 * the input clock to the __ADC__
 * @details
 * This enumeration maps to the __ADPS[2:0]__ of the __ADCSRA__ register
 * @note
 * By default, the successive approximation circuitry requires an input clock
 * requency between 50kHz to get maximum resolution.
 * If a lower resolution than 10 bits is needed, the input clock frequency to
 * the ADC can be higher than 200kHz to get a higher sample rate
 * @see ADC_init
 * @see ADC_get_prescaler
 * @see ADC_set_prescaler
 */
typedef enum
{
    ADC_PS2 = 0x01,  /**< Division factor 2 */
    ADC_PS4 = 0x02,  /**< Division factor 4 */
    ADC_PS8 = 0x03,  /**< Division factor 8 */
    ADC_PS16 = 0x04, /**< Division factor 16 */
    ADC_PS32 = 0x05, /**< Division factor 32 */
    ADC_PS64 = 0x06, /**< Division factor 64 */
    ADC_PS128 = 0x07 /**< Division factor 128 */
} adc_ps_t;

/**
 * @brief Define ADC Analog channel
 * @see ADC_enable_channel
 * @see ADC_disable_channel
 * @see ADC_get_channel
 * @see ADC_set_channel
 * @see ADC_read
 */
typedef enum
{
    ADC_CH0 = 0, /**< Input channel __ADC0__ */
    ADC_CH1 = 1, /**< Input channel __ADC1__ */
    ADC_CH2 = 2, /**< Input channel __ADC2__ */
    ADC_CH3 = 3, /**< Input channel __ADC3__ */
    ADC_CH4 = 4, /**< Input channel __ADC4__ */
    ADC_CH5 = 5, /**< Input channel __ADC5__ */
    ADC_CH6 = 6, /**< Input channel __ADC6__ */
    ADC_CH7 = 7  /**< Input channel __ADC7__ */
} adc_ch_t;

//------------------------------------------------------------------------------
// Basic Functions
//------------------------------------------------------------------------------

/**
 * @brief Initialize __Analog-to-Digital Converter (ADC)__
 * @param reference Voltage reference
 * @param resolution 8-bit or 10-bit result
 * @param prescaler Division factor
 * @see adc_ref_t
 * @see adc_bit_t
 * @see adc_ps_t
 * @ingroup adc_basic
 */
void ADC_init(adc_ref_t reference, adc_bit_t resolution, adc_ps_t prescaler);

/**
 * @brief Enable ADC channel for conversion
 * @param channel ADC channel to enable
 * @see adc_ch_t
 * @see adc_basic
 */
void ADC_enable_channel(adc_ch_t channel);

/**
 * @brief Disable ADC channel
 * @param channel ADC channel to disable
 * @see adc_ch_t
 * @see adc_basic
 */
#if defined(__AVR_ATmega328P__)
inline void ADC_disable_channel(adc_ch_t channel)
{
    if (ADC_CH5 >= channel)
    {
        BIT_set(DIDR0, BIT(channel));
    }
}
#endif

/**
 * @brief Start conversion, and return the converted value
 * @param channel Analog channel for the conversion
 * @return Result of the conversion
 * @note The ADC channel must be enabled with `ADC_enable_channel`
 * @see adc_ch_t
 * @ingroup adc_basic
 */
uint16_t ADC_read(adc_ch_t channel);

//------------------------------------------------------------------------------
// Advanced Functions
//------------------------------------------------------------------------------

/**
 * @brief Reset all ADC registers to their default values
 * @ingroup adc_advanced
 */
void ADC_reset(void);

/**
 * @brief Check whether the ADC is enabled
 * @return `TRUE` if the ADC is switched on, otherwise `FALSE`
 * @ingroup adc_advanced
 */
inline bool_t ADC_is_enabled(void)
{
    return (BIT_is_set(ADCSRA, BIT(ADEN)));
}

/**
 * @brief Enable ADC
 * @ingroup adc_advanced
 */
inline void ADC_enable(void)
{
    BIT_set(ADCSRA, BIT(ADEN)); // set `ADEN` flag
}

/**
 * @brief Disable ADC
 * @ingroup adc_advanced
 */
inline void ADC_disable(void)
{
    BIT_clear(ADCSRA, BIT(ADEN)); // clear `ADEN` flag
}

/**
 * @brief Return the ADC reference voltage
 * @return ADC reference voltage
 * @see adc_ref_t
 * @ingroup adc_advanced
 */
inline adc_ref_t ADC_get_reference(void)
{
    return (BIT_read(ADMUX, _ADC_MASK_REFERENCE));
}

/**
 * @brief Set ADC reference voltage
 * @param reference Voltage reference
 * @ingroup adc_advanced
 */
inline void ADC_set_reference(adc_ref_t reference)
{
    BIT_write(ADMUX, reference, _ADC_MASK_REFERENCE);
}

/**
 * @brief Return the ADC resolution
 * @return ADC resolution (8-bit or 10-bit)
 * @see adc_bit_t
 * @ingroup adc_advanced
 */
inline adc_bit_t ADC_get_resolution(void)
{
    return (BIT_read(ADMUX, _ADC_MASK_RESULT));
}

/**
 * @brief Set ADC resolution
 * @param resolution ADC resolution (8-bit or 10-bit)
 * @see adc_bit_t
 * @ingroup adc_advanced
 */
inline void ADC_set_resolution(adc_bit_t resolution)
{
    BIT_write(ADMUX, resolution, _ADC_MASK_RESULT);
}

/**
 * @brief Return the ADC prescaler
 * @return ADC prescaler
 * @see adc_ps_t
 * @ingroup adc_advanced
 */
inline adc_ps_t ADC_get_prescaler(void)
{
    return (BIT_read(ADCSRA, _ADC_MASK_PRESCALER));
}

/**
 * @brief Set ADC prescaler
 * @param prescaler ADC prescaler
 * @see adc_ps_t
 * @ingroup adc_advanced
 */
inline void ADC_set_prescaler(adc_ps_t prescaler)
{
    BIT_write(ADCSRA, prescaler, _ADC_MASK_PRESCALER);
}

/**
 * @brief Return the current active channel
 * @return Current ADC channel
 * @see adc_ch_t
 * @ingroup adc_advanced
 */
inline adc_ch_t ADC_get_channel(void)
{
    return (BIT_read(ADMUX, _ADC_MASK_CHANNEL));
}

/**
 * @brief Set ADC channel
 * @param channel ADC channel
 * @see adc_ch_t
 * @ingroup adc_advanced
 */
inline void ADC_set_channel(adc_ch_t channel)
{
    BIT_write(ADMUX, channel, _ADC_MASK_CHANNEL);
}

/**
 * @brief Check whether the conversion is complete
 * @return `TRUE` if the conversion is complete, otherwise `FALSE`
 */
inline bool_t ADC_is_complete(void)
{
    return (BIT_is_set(ADCSRA, BIT(ADIF))); // check `ADIF` flag
}

/**
 * @brief Start ADC conversion
 * @ingroup adc_advanced
 */
inline void ADC_start(void)
{
    BIT_set(ADCSRA, BIT(ADSC)); // set `ADSC` flag
}

/**
 * @brief Return the read value of the current active channel
 * @return Converted value (8-bit or 10-bit)
 * @ingroup adc_advanced
 */
uint16_t ADC_data(void);

#if defined(__AVR_ATmega328P__)
/**
 * @brief Enable ADC Auto Trigger
 * @warning Auto-trigger not available
 */
inline void ADC_enable_autotrigger(void)
{
    BIT_set(ADCSRA, BIT(ADATE));
}

/**
 * @brief Disable ADC Auto Trigger
 * @warning Auto-trigger not available
 */
inline void ADC_disable_autotrigger(void)
{
    BIT_clear(ADCSRA, BIT(ADATE));
}

#endif

/**
 * @brief Enable ADC interrupt
 * @warning Interruption not available
 */
inline void ADC_enable_interrupt(void)
{
    BIT_set(ADCSRA, BIT(ADIE)); // set `ADIE` flag
}

/**
 * @brief Disable ADC interrupt
 * @warning Interruption not available
 */
inline void ADC_disable_interrupt(void)
{
    BIT_clear(ADCSRA, BIT(ADIE)); // clear `ADIE` flag
}

/**
 * @brief Attach an interrupt handler to be called when an ADC conversion
 * complete occures
 * @param on_complete Function callback to call when the interruption occures.
 * @details
 * The callback function takes the value of the conversion as parameter
 */
void ADC_attach_interrupt(void (*on_complete)(uint16_t));

#endif // VEMAR_ADC_H

/**
 * @file adc.h
 * @brief ADC utility functions
 * @author Christian HUGON <chriss.hugon@gmail.com>
 * @version 1.0.0
 * @details
 * For quick configuration:
 * ```
 * void setup(void)
 * {
 *      ADC_init(ADC_AVCC, ADC_10BIT, ADC_PS128);
 *      ADC_enable_channel(ADC_CH0); // channel ADC0
 * }
 *
 * void loop(void)
 * {
 *      uint16 res = ADC_read(ADC_CH0);
 *      // handle res
 *      delay(1000); // wait for 1s
 * }
 * ```
 */

/**
 * @page page_adc ADC
 * @brief ADC (Analog-to-Digital Converter) module
 *
 * @section sec_adc_init Initialization
 * For advanced configuration:
 * - Set ADC reference voltage
 * - Set ADC prescaler
 * - Set ADC resolution
 * - Enable ADC
 * - Enable ADC channel
 * - Set ADC channel
 * - Start conversion
 * - Wait for conversion to complete
 * - Read ADC result
 */
