#ifndef VEMAR_COMMON_H
#define VEMAR_COMMON_H

#include <avr/io.h>

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
 * @brief Loop until the condition `cond` is satisfied
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

typedef unsigned char bool_t;

#endif // VEMAR_COMMON

/**
 * @file common.h
 * @brief Bit operations
 * @author Christian Hugon
 * @version 0.0.1
 */
