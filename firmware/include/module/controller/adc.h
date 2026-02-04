#ifndef VEMAR_ADC_H
#define VEMAR_ADC_H

#include "common.h"

/**
 * @defgroup adc_reference ADC Reference Voltage
 * @brief Available reference voltage selections for ADC
 * @see ADC_set_reference
 * @see ADC_init
 * @{
 */
#define ADC_REFERENCE_AREF 0x00	  ///< AREF, internal VREF turned off
#define ADC_REFERENCE_AVCC 0x40	  ///< AVCC with external capacitor at AREF pin
#define ADC_REFERENCE_INTERN 0xC0 ///< Internal 1.1V Voltage Reference
/**
 * @}
 */

/**
 * @defgroup adc_prescaler ADC Prescaler
 * @brief Available prescaler selections for ADC
 * @see ADC_set_prescaler
 * @see ADC_init
 * @{
 */
#define ADC_PRESCALER_2 0x01   ///< ADC Prescaler 2
#define ADC_PRESCALER_4 0x02   ///< ADC Prescaler 4
#define ADC_PRESCALER_8 0x03   ///< ADC Prescaler 8
#define ADC_PRESCALER_16 0x04  ///< ADC Prescaler 16
#define ADC_PRESCALER_32 0x05  ///< ADC Prescaler 32
#define ADC_PRESCALER_64 0x06  ///< ADC Prescaler 64
#define ADC_PRESCALER_128 0x07 ///< ADC Prescaler 128
/**
 * @}
 */

/**
 * @defgroup adc_result ADC Left Adjust Result
 * @brief 10-bit or 8-bit result for ADC
 * @see ADC_set_result
 * @see ADC_init
 * @{
 */
#define ADC_RESULT_10 0x00 ///< ADC 10-bit result
#define ADC_RESULT_8 0x20  ///< ADC 8-bit result
/**
 * @}
 */

/**
 * @defgroup adc_channel ADC Channel
 * @brief Available channel selections for ADC
 * @see ADC_set_channel
 * @see ADC_enable_channel
 * @see ADC_disable_channel
 * @see ADC_read
 * @{
 */
#define ADC_CHANNEL_0 0x00 ///< ADC0
#define ADC_CHANNEL_1 0x01 ///< ADC1
#define ADC_CHANNEL_2 0x02 ///< ADC2
#define ADC_CHANNEL_3 0x03 ///< ADC3
#define ADC_CHANNEL_4 0x04 ///< ADC4
#define ADC_CHANNEL_5 0x05 ///< ADC5

#define ADC_CHANNEL_6 0x06 ///< ADC6
#define ADC_CHANNEL_7 0x07 ///< ADC7
/**
 * @}
 */

/**
 * @{
 * @brief ADC Masks for internal use
 */
#define ADC_MASK_REFERENCE 0xC0
#define ADC_MASK_PRESCALER 0x07
#define ADC_MASK_RESULT 0x20
#define ADC_MASK_CHANNEL 0x0F
/**
 * @}
 */

//------------------------------------------------------------------------------
// ADMUX --- ADC Multiplexer Selection Register
// | REFS1 | REFS0 | ADLAR | --- | MUX3 | MUX2 | MUX1 | MUX0 |
//------------------------------------------------------------------------------

/**
 * @brief Set ADC reference voltage
 * @param reference Voltage reference
 * - ADC_REFERENCE_AREF
 * - ADC_REFERENCE_AVCC
 * - ADC_REFERENCE_INTERN
 *
 * @details
 * The `REFS[1:0]` bits select the voltage reference for the ADC.
 * If these bits are changed during a conversion, the change will not go in
 * effect until this conversion is complete (`ADIF` in `ADCSRA` is set).
 * The iunternal voltage reference options may not be used if an external
 * reference voltage is being applied to the `AREF` pin.
 */
inline void ADC_set_reference(byte_t reference)
{
	BIT_write(ADMUX, reference, ADC_MASK_REFERENCE);
}

/**
 * @brief Set ADC Left Adjust Result
 * @param result ADC Left Adjust Result
 * - ADC_RESULT_10
 * - ADC_RESULT_8
 *
 * @details
 * The `ADLAR` bit affects the presentation of the ADC conversion result in
 * the ADC data register. Write one to the `ADLAR` to left adjust the result.
 * Otherwise, the result is right adjusted. Changing the `ADLAR` bit will affect
 * the ADC data register immediately, regardless of any ongoing conversion.
 */
inline void ADC_set_result(byte_t result)
{
	BIT_write(ADMUX, result, ADC_MASK_RESULT);
}

/**
 * @brief Set ADC channel
 * @param channel ADC channel
 * @details
 * The value of `MUX[3:0]` selects which analog inputs are connected to the ADC.
 * If these bits are changed during a conversion, the change will not go
 * in effect until this conversion is complete (`ADIF` in `ADCSRA` is set).
 * @see adc_channel
 */
inline void ADC_set_channel(byte_t channel)
{
	BIT_write(ADMUX, channel, ADC_MASK_CHANNEL);
}

//------------------------------------------------------------------------------
// ADCSRA -- ADC Control and Status Register A
// | ADEN | ADSC | ADATE | ADIF | ADIE | ADPS2 | ADPS1 | ADPS0 |
//------------------------------------------------------------------------------

/**
 * @brief Enable ADC
 *
 * @details
 * Writing `ADEN` to `1` enables the ADC.
 */
inline void ADC_enable(void)
{
	BIT_set(ADCSRA, BIT(ADEN));
}

/**
 * @brief Disable ADC
 * @details
 * By writing `ADEN` to `0`, the ADC is turned off. Turning the ADC off while
 * a conversion is in progress, will terminate this conversion.
 */
inline void ADC_disable(void)
{
	BIT_clear(ADCSRA, BIT(ADEN));
}

/**
 * @brief Start ADC conversion
 * @details
 * In single conversion mode, write `ADSC` bit to one to start each conversion.
 * In free running mode, write this bit to one to start the first conversion.
 * The first conversion after `ADSC` has been written after the ADC has been
 * enabled, or if `ADSC` is written at the same time as the ADC is enabled,
 * will take 25 ADC clock cycles instead of the normal 13.
 * This first conversion performs initialization of the ADC.
 *
 * `ADSC` will read as one as long as a conversion is in progress.
 * When the conversion is complete, it returns to zero.
 * Writing zero to this bit has no effect.
 */
inline void ADC_start_conversion(void)
{
	BIT_set(ADCSRA, BIT(ADSC));
}

/**
 * @brief Enable ADC Auto Trigger
 * @details
 * When `ADATE` bit is written to `1`, Auto Triggering of the ADC is enabled.
 * The ADC will start a conversion on a positive edge of the selected trigger
 * signal. The trigger source is selected by setting the ADC Trigger Select
 * `ADTS` in `ADCSRB`.
 */
inline void ADC_enable_autotrigger(void)
{
	BIT_set(ADCSRA, BIT(ADATE));
}

/**
 * @brief Disable ADC Auto Trigger
 * @see ADC_enable_autotrigger
 */
inline void ADC_disable_autotrigger(void)
{
	BIT_clear(ADCSRA, BIT(ADATE));
}

/**
 * @details
 * `ADIF` bit is set when an ADC conversion completes and the Data Registers
 * are updated. The ADC Conversion Complete Interrupt is executed if the `ADIE`
 * bit and the I-bit in SREG are set. `ADIF` is cleared by hardware when
 * executing the corresponding interrupt handling vector. Alternatively, `ADIF`
 * is cleared by writing a logical `1` to the flag.
 * Beware that if doing a Read-Modify-Write on `ADCSRA` a pending interrupt can
 * be disabled. This also applies if the `SBI` and `CBI` instructions are used.
 */
inline bool_t ADC_is_conversion_complete(void)
{
	return (BIT_read(ADCSRA, BIT(ADIF)));
}

/**
 * @brief Enable ADC interrupt
 * @details
 * When this bit written to `1` and the I-bit in `SREG` is set,
 * the ADC conversion complete interrupt is activated.
 */
inline void ADC_enable_interrupt(void)
{
	BIT_set(ADCSRA, BIT(ADIE));
}

/**
 * @brief Disable ADC interrupt
 */
inline void ADC_disable_interrupt(void)
{
	BIT_clear(ADCSRA, BIT(ADIE));
}

/**
 * @brief Set ADC prescaler
 * @param prescaler ADC prescaler
 * @details
 * The ADPS[2:0] bits determine the division factor between the system clock
 * frequency and the input clock to the ADC.
 * @see adc_prescaler
 */
inline void ADC_set_prescaler(byte_t prescaler)
{
	BIT_write(ADCSRA, prescaler, ADC_MASK_PRESCALER);
}

//------------------------------------------------------------------------------
// ADCL and ADCH --- The ADC Data Register
//------------------------------------------------------------------------------

/**
 * @details
 * When ADC conversion is complete, the result is found in `ADCL` and `ADCH`.
 * When `ADCL` is read, the ADC Data Register is not updated until ADCH is read.
 * Consequently, if the result is left adjusted and no more than 8-bit precision
 * is required, it is sufficient to read `ADCH`. Otherwise, `ADCL` must be read
 * first, then `ADCH`.
 * The `ADLAR` bit in `ADMUX`, and the MUXn bits in `ADMUX` affect the way
 * the result is read from the register.
 * If `ADLAR` is set, the result is left adjusted.
 * If `ADLAR is cleared (default), the result is right adjusted.
 */
unsigned int ADC_data(void);

//------------------------------------------------------------------------------
// ADCSRB --- ADC Control and Status Register B
// | --- | ACME | --- | --- | --- | ADTS2 | ADTS1 | ADTS0 |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DIDR0 --- Digital Input Register 0
// | --- | --- | ADC5D | ADC4D | ADC3D | ADC2D | ADC1D | ADC0D |
//------------------------------------------------------------------------------

/**
 * @brief Disable ADC channel
 * @param channel ADC channel
 * - ADC_CHANNEL_0
 * - ADC_CHANNEL_1
 * - ADC_CHANNEL_2
 * - ADC_CHANNEL_3
 * - ADC_CHANNEL_4
 * - ADC_CHANNEL_5
 *
 * @details
 * When ADC[5:0]D bit is written logic `1`, the digital input buffer on
 * the corresponding ADC pin disabled. The corresponding PIN regiester bit
 * will always read as `0` when this bit is set. When an analog signal
 * is applied to then ADC[5:0] pin and the digital input from this pin
 * is not needed, this should be written logic `1` to reduce power consumption
 * in the digital input buffer.
 */

inline void ADC_disable_channel(byte_t channel)
{
	BIT_set(DIDR0, BIT(channel));
}

/**
 * @brief Enable ADC channel
 * @param channel ADC channel
 * @see adc_channel
 */
inline void ADC_enable_channel(byte_t channel)
{
		BIT_clear(DIDR0, BIT(channel));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * @brief Reset all ADC registers to default values
 */
void ADC_reset(void);

/**
 * @brief Initialize ADC (Analog-to-Digital Converter)
 * @param reference Voltage reference
 * @param prescaler Prescaler
 * @param result ADC Left Adjust Result, 8-bit or 10-bit result
 * @see adc_reference
 * @see adc_prescaler
 * @see adc_result
 */
void ADC_init(byte_t reference, byte_t prescaler, byte_t result);

/**
 * @brief Start conversion, and return the converted value
 * @param channel Analog channel for the conversion
 * @return Result of the conversion
 * @see adc_channel
 */
unsigned int ADC_read(byte_t channel);

#endif // VEMAR_ADC_H

/**
 * @file adc.h
 * @brief ADC manipulation
 * @author Christian Hugon
 * @version 0.0.1
 */

/**
 * @page page_adc ADC
 * @brief ADC (Analog-to-Digital Converter) module
 *
 * @section sec_adc_init Initialization
 * - Enable ADC
 * - Set ADC reference voltage
 * - Set ADC prescaler
 * - Set ADC result
 * - Enable ADC channel
 * - Set ADC channel
 * - Start conversion
 * - Wait for conversion to complete
 * - Read ADC result
 */
