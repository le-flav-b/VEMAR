#ifndef VEMAR_UART_H
#define VEMAR_UART_H

#include "common.h"

/// Equation for calculating `UBRR` value (Asynchronous Double Speed mode)
#define UART_UBRR(baud) (F_CPU / 8UL / baud - 1)

#define UART_BAUDRATE_9600 UART_UBRR(9600UL)     ///< Baud rate 9600
#define UART_BAUDRATE_115200 UART_UBRR(115200UL) ///< Baud rate 115200

/**
 * @brief UART frame format: UART_FORMAT_<bits><parity><stop>
 * * bits: data bits (5, 6, 7, 8)
 * * parity: parity mode (none, even, odd)
 * * stop: stop bits (1, 2)
 */
#define UART_FORMAT_5N1 0x00
#define UART_FORMAT_6N1 0x02
#define UART_FORMAT_7N1 0x04
#define UART_FORMAT_8N1 0x06
#define UART_FORMAT_5N2 0x08
#define UART_FORMAT_6N2 0x0A
#define UART_FORMAT_7N2 0x0C
#define UART_FORMAT_8N2 0x0E
#define UART_FORMAT_5E1 0x20
#define UART_FORMAT_6E1 0x22
#define UART_FORMAT_7E1 0x24
#define UART_FORMAT_8E1 0x26
#define UART_FORMAT_5E2 0x28
#define UART_FORMAT_6E2 0x2A
#define UART_FORMAT_7E2 0x2C
#define UART_FORMAT_8E2 0x2E
#define UART_FORMAT_5O1 0x30
#define UART_FORMAT_6O1 0x32
#define UART_FORMAT_7O1 0x34
#define UART_FORMAT_8O1 0x36
#define UART_FORMAT_5O2 0x38
#define UART_FORMAT_6O2 0x3A
#define UART_FORMAT_7O2 0x3C
#define UART_FORMAT_8O2 0x3E

/**
 * @brief UART initialization
 * @param baudrate UART baud rate
 * @param format UART format format, use UART_FORMAT_8N1 as default
 */
void UART_init(unsigned long baudrate, byte_t format);

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
 * @brief Check whether receive is complete
 * @return Non-zero value if the receive is complete, otherwise `0`
 * @details
 * `RXC0` flag bit is set when there are unread data in the receive buffer
 * and cleared when the receive buffer is empty (i.e., does not contain any
 * unread data). If the Receiver is disabled, the receive buffer will be
 * flushed and consequently the `RXC0` bit will become `0`.
 * The `RXC0` flag can be used to generate a _Receive Complete interrupt_.
 */
inline bool_t UART_is_rx_complete(void)
{
    return (BIT_read(UCSR0A, BIT(RXC0)));
}

/**
 * @brief Check whether transmit is complete
 * @return Non-zero value if the transmit is complete, otherwise `0`
 * @details
 * `TXC0` flag bit is set when the entire frame in the Transmit Shift Register
 * has been shifted out and there are no new data currently present in
 * the transmit buffer (`UDR0`). The `TXC0` flag bit is automatically cleared
 * when a transmit complete interrupt is executed, or it can be cleared by
 * writing a `1` to its bit location.
 * The `TXC0` flag can generate a _Transmit Complete interrupt_.
 */
inline bool_t UART_is_tx_complete(void)
{
    return (BIT_read(UCSR0A, BIT(TXC0)));
}

/**
 * @details
 * `FE0` is set if the next character in the receive buffer has a Frame Error
 * when received (i.e., when the first stop bit of the next character in
 * the receive buffer is zero).
 * This bit is valid until the receive buffer (`UDR0`) is read. The `FE0` bit
 * is `0` when the stop bit of received data is `1`.
 * Always set this bit to `0` when writing `UCSR0A`.
 */
inline bool_t UART_has_frame_error(void)
{
    return (BIT_read(UCSR0A, BIT(FE0)));
}

/**
 * @brief Double the UART Transmission speed
 * @details
 * The `U2X0` bit only has effect for the asynchronous operation.
 * Write this bit to `0` when using synchronous operation.
 * Writing this to `1` will reduce the divisor of the baud rate divider from
 * 16 to 8 effectively doubling the transfer rate for asynchronous communication.
 */
inline void UART_enable_double_speed(void)
{
    BIT_set(UCSR0A, BIT(U2X0));
}

/**
 * @brief Enable Receive Complete Interrupt
 * @details
 * Writing `RXCIE0` bit to `1` enables interrupt on the `RXC0` flag.
 * A UART Receive Complete interrupt will be generated only if:
 * - `RXCIE0` bit is written to `1`
 * - Global Interrupt Flag in `SREG` is written to `1`
 * - `RXC0` bit in `UCSR0A` is set
 */
inline void UART_enable_rxci(void)
{
    BIT_set(UCSR0B, BIT(RXCIE0));
}

/**
 * @brief Disable Receive Complete Interrupt
 */
inline void UART_disable_rxci(void)
{
    BIT_clear(UCSR0B, BIT(RXCIE0));
}

/**
 * @brief Enable Transmit Complete Interrupt
 * @details
 * Writing `TXCIE0` bit to `1` enables interrupt on the `TXC0` flag.
 * A UART Transmit Complete interrupt will be generated only if:
 * - `TXCIE0` bit is written to `1`
 * - Global Interrupt Flag in `SREG` is written to `1`
 * - `TXC0` bit in `UCSR01` is set
 */
inline void UART_enable_txci(void)
{
    BIT_set(UCSR0B, BIT(TXCIE0));
}

/**
 * @brief Disable Transmit Complete Interrupt
 */
inline void UART_disable_txci(void)
{
    BIT_clear(UCSR0B, BIT(TXCIE0));
}

/**
 * @details
 * The `UDRE0` flag indicates if the transmit buffer (`UDR0`) is ready to
 * receive new data. if `UDRE0` is `1`, the buffer is empty, and therefore
 * ready to be written.
 * The `UDRE0` flag can generate a Data Register Empty interrupt.
 * `UDR0` is set after a reset to indicate that the Transmitter is ready.
 */
inline bool_t UART_is_data_empty(void)
{
    return (BIT_read(UCSR0A, BIT(UDRE0)));
}

/**
 * @brief Flush the receiver buffer
 */
void UART_flush(void);

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
 * @author Christian Hugon
 * @version 0.0.1
 */

/**
 * @page UART
 * @brief UART module
 * @section sec_uart_init Initialization
 * @par
 * The UART has to be initialized before any communication can take place.
 * The initializatio process normally consists of:
 * - Setting the _baud rate_
 * - Setting _frame format_
 * - Enabling the _Transmitter_ and/or the _Receiver_ depending on the usage
 */
