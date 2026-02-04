#ifndef VEMAR_PRINT_H
#define VEMART_H

#include "uart.h"

#define PRINTLN(value, type) \
	PRINT_##type(value);     \
	PRINT_char('\r');        \
	PRINT_char('\n')

/**
 * @brief Initialize the serial interface
 * @details This function initializes the serial interface with the follwing
 * parameters:
 * - Baud rate of 115200
 * - Frame format 8N1
 */
inline void PRINT_init(void)
{
	UART_init(UART_BAUDRATE_115200, UART_FORMAT_8N1);
}

/**
 * @brief Transmit a single character over the serial interface
 * @param ch The character to transmit
 */
inline void PRINT_char(char ch)
{
	UART_transmit((byte_t)ch);
}

/**
 * @brief Transmit a string over the serial interface
 * @param str The string to transmit
 */
void PRINT_string(const char *str);

void PRINT_int(int nbr);

void PRINT_uint(unsigned int nbr);

/**
 * @brief Transmit a number over the serial interface
 * @param nbr The number to transmit
 */
void PRINT_long(long nbr);

void PRINT_ulong(unsigned long nbr);

void PRINT_hex(unsigned long nbr);

#endif // VEMART_H

/**
 * @file print.h
 * @brief Print functions
 * @version 0.0.1
 * @author Christian Hugon
 */
