#ifndef VEMAR_UART_H
#define VEMAR_UART_H

#include "common.h"

/**
 * @brief UART serial format: UART_SERIAL_<bits><parity><stop>
 * * bits: data bits (5, 6, 7, 8)
 * * parity: parity mode (none, even, odd)
 * * stop: stop bits (1, 2)
 */
#define UART_SERIAL_5N1 0x00
#define UART_SERIAL_6N1 0x02
#define UART_SERIAL_7N1 0x04
#define UART_SERIAL_8N1 0x06
#define UART_SERIAL_5N2 0x08
#define UART_SERIAL_6N2 0x0A
#define UART_SERIAL_7N2 0x0C
#define UART_SERIAL_8N2 0x0E
#define UART_SERIAL_5E1 0x20
#define UART_SERIAL_6E1 0x22
#define UART_SERIAL_7E1 0x24
#define UART_SERIAL_8E1 0x26
#define UART_SERIAL_5E2 0x28
#define UART_SERIAL_6E2 0x2A
#define UART_SERIAL_7E2 0x2C
#define UART_SERIAL_8E2 0x2E
#define UART_SERIAL_5O1 0x30
#define UART_SERIAL_6O1 0x32
#define UART_SERIAL_7O1 0x34
#define UART_SERIAL_8O1 0x36
#define UART_SERIAL_5O2 0x38
#define UART_SERIAL_6O2 0x3A
#define UART_SERIAL_7O2 0x3C
#define UART_SERIAL_8O2 0x3E

/**
 * @brief UART initialization
 * @param baudrate UART baud rate
 * @param serial UART serial format, use UART_SERIAL_8N1 as default
 */
void UART_init(unsigned long baudrate, byte_t serial);

/**
 * @brief Transmit a byte of data
 * @param data Byte to transmit
 */
void UART_transmit(byte_t data);

/**
 * @brief Receive a byte of data
 * @return Received byte
 */
byte_t UART_receive(void);

/**
 * @brief Transmit a string
 * @param str Null-terminated string to transmit
 */
void UART_print(const char *str);

/**
 * @brief Transmit a string with newline
 * @param str Null-terminated string to transmit
 */
void UART_println(const char *str);

#endif // VEMAR_UART_H

/**
 * @file uart.h
 * @brief UART module header file
 */
