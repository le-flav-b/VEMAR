#ifndef VEMAR_COMMON_H
#define VEMAR_COMMON_H

#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define DEBOUNCE_TIME 30

#define delay(ms) _delay_ms(ms)

/**
 * @brief Create a bitmask with a single bit set to `1` at the position
 * specified by the parameter `n`
 * @param n Position of the bit to set
 */
#define BIT(n) (1 << (n))

/**
 * @brief Set (write to `1`) the specific bits of `reg`
 * @param reg Register whose bits to set
 * @param bit Bits to set
 */
#define BIT_set(reg, bit) (reg |= (bit))

/**
 * @brief Clear (write to `0`) the specific bits of `reg`
 * @param reg Register whose bits to clear
 * @param bit Bits to clear
 */
#define BIT_clear(reg, bit) (reg &= ~(bit))

/**
 * @brief Toggle the specific bits of `reg`
 * @param reg Register whose bits to toggle
 * @param bit Bits to toggle
 */
#define BIT_toggle(reg, bit) (reg ^= (bit))

/**
 * @brief Read specific bits of `reg`
 * @param reg Register to read
 * @param mask Mask to apply
 */
#define BIT_read(reg, mask) (reg & (mask))

/**
 * @brief Write value at specific position of `reg`
 * @param reg Register whose bits to write
 * @param value Value to write
 * @param mask Mask to apply
 */
#define BIT_write(reg, value, mask) (reg = ((value) | (reg & ~(mask))))

/**
 * @brief Loop until the specific condition is met.
 * @param cond Condition to end the loop
 */
#define WAIT_UNTIL(cond) \
	do                   \
	{                    \
	} while (!(cond))

/**
 * @brief 8-bit register
 */
typedef unsigned char byte_t;

/**
 * @brief Boolean type
 */
typedef unsigned char bool_t;

/**
 * @brief
 */
typedef unsigned char length_t;

#define LENGTH_MIN 0
#define LENGTH_MAX 0xFF

#endif // VEMAR_COMMON

/**
 * @file common.h
 * @brief Bit operations
 * @author Christian Hugon
 * @version 0.0.1
 */
