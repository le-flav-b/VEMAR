#ifndef VEMAR_SERIAL_H
#define VEMAR_SERIAL_H

#include "common.h"
#include "uart.h"

/**
 * @brief Print specified value
 * @param value Value to display
 * @param type Type of the value
 * - char (single ASCII character)
 * - int/uint (signed or unsigned 16-bit number)
 * - long/ulong (signed or unsigned 32-bit number)
 * - hex (hexadecimal number)
 * - str (null-terminated string)
 */
#define SERIAL_print(value, type) \
    SERIAL_print_##type(value)

/**
 * @brief Print specific value followd by line feed character
 *
 * @see SERIAL_print
 */
#define SERIAL_println(value, type) \
    SERIAL_print_##type(value);     \
    UART_transmit('\r');            \
    UART_transmit('\n')

/**
 * @brief Convert input to specific type
 * @param value_ptr Address of the variable to store
 * @param type Type of the variable
 * - char
 * - int/uint
 * - long/ulong
 * - str
 */
#define SERIAL_scan(value_ptr, type) \
    SERIAL_scan_##type(value_ptr)

/**
 * @brief Initialize the Serial Communication
 */
void SERIAL_init(void);

/**
 * @brief The `SERIAL_print_` family of functions display specific typed
 * variable on the terminal
 */
void SERIAL_print_char(char ch);
void SERIAL_print_int(int n);
void SERIAL_print_uint(unsigned int n);
void SERIAL_print_long(long n);
void SERIAL_print_ulong(unsigned long n);
void SERIAL_print_hex(unsigned long n);
void SERIAL_print_str(const char *str);

/**
 * @brief The `SERIAL_scan_` family of functions allow to retrieve specific
 * typed variable from the terminal
 *
 * Undefined behavior when overflow/underflow occurs
 */
void SERIAL_scan_char(char *ch);
void SERIAL_scan_int(int *n);
void SERIAL_scan_uint(unsigned int *n);
void SERIAL_scan_long(long *n);
void SERIAL_scan_ulong(unsigned long *n);
void SERIAL_scan_str(char *str);

#endif // VEMAR_SERIAL_H
