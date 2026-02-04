#ifndef VEMAR_PIN_H
#define VEMAR_PIN_H

#include "common.h"

#define PIN_PB0 0x00
#define PIN_PB1 0x01
#define PIN_PB2 0x02
#define PIN_PB3 0x03
#define PIN_PB4 0x04
#define PIN_PB5 0x05
#define PIN_PB6 0x06
#define PIN_PB7 0x07

#define PIN_PC0 0x30
#define PIN_PC1 0x31
#define PIN_PC2 0x32
#define PIN_PC3 0x33
#define PIN_PC4 0x34
#define PIN_PC5 0x35
#define PIN_PC6 0x36
#define PIN_PC7 0x37

#define PIN_PD0 0x60
#define PIN_PD1 0x61
#define PIN_PD2 0x62
#define PIN_PD3 0x63
#define PIN_PD4 0x64
#define PIN_PD5 0x65
#define PIN_PD6 0x66
#define PIN_PD7 0x67

#define PIN_INPUT 0
#define PIN_OUTPUT 1

#define PIN_LOW 0
#define PIN_HIGH 1

/**
 * @brief
 * @param pin
 */
byte_t PIN_read(byte_t pin);

/**
 * @brief
 * @param pin
 * @param state
 */
void PIN_write(byte_t pin, byte_t state);

/**
 * @brief
 * @param pin
 */
void PIN_toggle(byte_t pin);

/**
 * @brief
 * @param pin
 * @param mode
 */
void PIN_mode(byte_t pin, byte_t mode);

#endif // VEMAR_PIN_H
