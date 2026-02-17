#ifndef VEMAR_UART_H
#define VEMAR_UART_H

#include "common.h"

/// Equation for calculating `UBRR` value (Asynchronous Double Speed mode)
#define UART_UBRR(baud) (F_CPU / 8UL / baud - 1)

/**
 * @defgroup uart_baudrate UART Baud Rate
 * @brief Available baud rate selections for UART
 * @see UART_init
 * @{
 */
#define UART_BAUDRATE_9600 UART_UBRR(9600UL)	 ///< Baud rate 9600
#define UART_BAUDRATE_115200 UART_UBRR(115200UL) ///< Baud rate 115200
/**
 * @}
 */

/**
 * @brief UART Frame Format selections
 *
 * UART_FORMAT_&lt;bits&gt;&lt;parity&gt;&lt;stop&gt;
 * - __bits__: data bits (5, 6, 7, 8)
 * - __parity__: parity mode (none, even, odd)
 * - __stop__: stop bits (1, 2)
 */
typedef enum vemar_uart_format
{
	UART_5N1 = 0x00, ///< 5 Data bits, no parity, 1 Stop bit
	UART_6N1 = 0x02, ///< 6 Data bits, no parity, 1 Stop bit
	UART_7N1 = 0x04, ///< 7 Data bits, no parity, 1 Stop bit
	UART_8N1 = 0x06, ///< 8 Data bits, no parity, 1 Stop bit
	UART_5N2 = 0x08, ///< 5 Data bits, no parity, 2 Stop bit
	UART_6N2 = 0x0A, ///< 6 Data bits, no parity, 2 Stop bit
	UART_7N2 = 0x0C, ///< 7 Data bits, no parity, 2 Stop bit
	UART_8N2 = 0x0E, ///< 8 Data bits, no parity, 2 Stop bit
	UART_5E1 = 0x20, ///< 5 Data bits, even parity, 1 Stop bit
	UART_6E1 = 0x22, ///< 6 Data bits, even parity, 1 Stop bit
	UART_7E1 = 0x24, ///< 7 Data bits, even parity, 1 Stop bit
	UART_8E1 = 0x26, ///< 8 Data bits, even parity, 1 Stop bit
	UART_5E2 = 0x28, ///< 5 Data bits, even parity, 2 Stop bit
	UART_6E2 = 0x2A, ///< 6 Data bits, even parity, 2 Stop bit
	UART_7E2 = 0x2C, ///< 7 Data bits, even parity, 2 Stop bit
	UART_8E2 = 0x2E, ///< 8 Data bits, even parity, 2 Stop bit
	UART_5O1 = 0x30, ///< 5 Data bits, odd parity, 1 Stop bit
	UART_6O1 = 0x32, ///< 6 Data bits, odd parity, 1 Stop bit
	UART_7O1 = 0x34, ///< 7 Data bits, odd parity, 1 Stop bit
	UART_8O1 = 0x36, ///< 8 Data bits, odd parity, 1 Stop bit
	UART_5O2 = 0x38, ///< 5 Data bits, odd parity, 2 Stop bit
	UART_6O2 = 0x3A, ///< 6 Data bits, odd parity, 2 Stop bit
	UART_7O2 = 0x3C, ///< 7 Data bits, odd parity, 2 Stop bit
	UARTT_8O2 = 0x3E ///< 8 Data bits, odd parity, 2 Stop bit
} uart_format_t;

/**
 * @brief UART operation mode selection
 */
typedef enum vemar_enum_uart_mode
{
	UART_TX = BIT(TXEN0), ///< Transmitter
	UART_RX = BIT(RXEN0)  ///< Receiver
} uart_mode_t;

/**
 * @brief UART interruption selections
 */
typedef enum vemar_enum_uart_interrupt
{
	UART_TX_COMPLETE = BIT(TXCIE0), ///< Interrupt on TX complete
	UART_RX_COMPLETE = BIT(RXCIE0)	///< Interrupt on RX complete
} uart_interrupt_t;

/**
 * @brief UART initialization
 * @param baudrate UART baud rate.
 * @param format UART format, it is advised to use UART_FORMAT_8N1 as default.
 * @param mode UART operation mode: transmission or reception; can be OR'ed
 * @see uart_baudrate
 * @see uart_format
 */
void UART_init(unsigned long baudrate, uart_format_t format, uart_mode_t mode);

/**
 * @brief Set UART baud rate
 * @param baudrate Baud rate
 */
inline void UART_set_baudrate(unsigned long baudrate)
{
	UBRR0H = (byte_t)(baudrate >> 8); // 4 most significant bits
	UBRR0L = (byte_t)(baudrate);	  // 8 least significant bits
}

/**
 * @brief Set UART frame format
 * @param format Frame format
 */
inline void UART_set_format(uart_format_t format)
{
	UCSR0C = (byte_t)format;
}

/**
 * @brief Enable UART receiver
 */
inline void UART_enable_receiver(void)
{
	BIT_set(UCSR0B, BIT(RXEN0));
}

/**
 * @brief Disable UART receiver
 */
inline void UART_disable_receiver(void)
{
	BIT_clear(UCSR0B, BIT(RXEN0));
}

/**
 * @brief Enable UART transmitter
 */
inline void UART_enable_transmitter(void)
{
	BIT_set(UCSR0B, BIT(TXEN0));
}

/**
 * @brief Disable UART transmitter
 */
inline void UART_disable_transmitter(void)
{
	BIT_clear(UCSR0B, BIT(TXEN0));
}

/**
 * @brief Enable UART 
 */
void UART_enable_interrupt(uart_interrupt_t interrupt);

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
 * - Enabling the double speed (if neccessary)
 */
