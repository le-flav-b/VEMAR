#ifndef VEMAR_ADC_H
#define VEMAR_ADC_H

#include "common.h"

// ADC Reference Voltage Settings
#define ADC_REFERENCE_AREF 0x00   ///< AREF, internal VREF turned off
#define ADC_REFERENCE_AVCC 0x40   ///< AVCC with external capacitor at AREF pin
#define ADC_REFERENCE_INTERN 0xC0 ///< Internal 1.1V Voltage Reference

// ADC Prescaler Settings
#define ADC_PRESCALER_2 0x01   ///< ADC Prescaler 2
#define ADC_PRESCALER_4 0x02   ///< ADC Prescaler 4
#define ADC_PRESCALER_8 0x03   ///< ADC Prescaler 8
#define ADC_PRESCALER_16 0x04  ///< ADC Prescaler 16
#define ADC_PRESCALER_32 0x05  ///< ADC Prescaler 32
#define ADC_PRESCALER_64 0x06  ///< ADC Prescaler 64
#define ADC_PRESCALER_128 0x07 ///< ADC Prescaler 128

// ADC Left Adjust Result Settings
#define ADC_RESULT_10 0x00 ///< ADC 10-bit result
#define ADC_RESULT_8 0x20  ///< ADC 8-bit result

// ADC Channel Selections
#define ADC_CHANNEL_0 0x00 ///< ADC0
#define ADC_CHANNEL_1 0x01 ///< ADC1
#define ADC_CHANNEL_2 0x02 ///< ADC2
#define ADC_CHANNEL_3 0x03 ///< ADC3
#define ADC_CHANNEL_4 0x04 ///< ADC4
#define ADC_CHANNEL_5 0x05 ///< ADC5
#define ADC_CHANNEL_6 0x06 ///< ADC6
#define ADC_CHANNEL_7 0x07 ///< ADC7

/**
 * @brief Initialize ADC (Analog-to-Digital Converter)
 * @param reference Voltage reference
 * @param prescaler Prescaler
 * @param result ADC Left Adjust Result, 8-bit or 10-bit result
 */
void ADC_init(byte_t reference, byte_t prescaler, byte_t result);

/**
 * @brief Start conversion, and return the converted value
 * @param channel Analog channel for the conversion
 * @return Result of the conversion
 */
unsigned int ADC_read(byte_t channel);

#endif // VEMAR_ADC_H
