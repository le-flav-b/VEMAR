#ifndef GASSES_H
#define GASSES_H

#include "i2c.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

// I2C slave address (atmosphere uses 0x09)
#define SLAVE_ADDR          0x0A

// Payload: CO2(2) + CO(2) + NH3(2) + NO2(2) + O2(2) + TEMP(1) + STATUS(1) = 12 bytes
// STATUS byte: bit 0 = co2_valid (CRC passed), bit 1 = co2_preheating (< 60s uptime)
// TEMP byte  : raw response[4] from MH-Z1911A — subtract 44 to get °C
#define GAS_PACKET_SIZE         12
#define CO2_TEMP_OFFSET         44    // from MHZ library: temperature = response[4] - 44
#define CO2_PREHEAT_MS          60000 // MHZ1911A_PREHEATING_TIME per reference library

// MH-Z1911A CO2 sensor UART response length
#define CO2_RESPONSE_LEN    9

// ADC MUXPOS channels (ATtiny1614)
#define ADC_O2   ADC_MUXPOS_AIN4_gc   // PA4 - O2 sensor (via op-amp)
#define ADC_CO   ADC_MUXPOS_AIN5_gc   // PA5 - MICS-6814 CO
#define ADC_NH3  ADC_MUXPOS_AIN6_gc   // PA6 - MICS-6814 NH3
#define ADC_NO2  ADC_MUXPOS_AIN7_gc   // PA7 - MICS-6814 NO2

#define wdt_reset() __asm__ __volatile__ ("wdr")

void uart_init(void);
void adc_init(void);
void slave_init(void);

#endif // GASSES_H
