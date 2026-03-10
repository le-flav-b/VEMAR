#ifndef VEMAR_UART_H
#define VEMAR_UART_H

#include "common.h"

/**
 * @brief Define UART Frame Format
 * @details
 * UART_FORMAT_&lt;bits&gt;&lt;parity&gt;&lt;stop&gt;
 * - __bits__: data bits (5, 6, 7, 8)
 * - __parity__: parity mode (none, even, odd)
 * - __stop__: stop bits (1, 2)
 */
typedef enum
{
    UART_5N1 = 0x00, /**< 5 Data bits, no parity, 1 Stop bit */
    UART_6N1 = 0x02, /**< 6 Data bits, no parity, 1 Stop bit */
    UART_7N1 = 0x04, /**< 7 Data bits, no parity, 1 Stop bit */
    UART_8N1 = 0x06, /**< 8 Data bits, no parity, 1 Stop bit */
    UART_5N2 = 0x08, /**< 5 Data bits, no parity, 2 Stop bit */
    UART_6N2 = 0x0A, /**< 6 Data bits, no parity, 2 Stop bit */
    UART_7N2 = 0x0C, /**< 7 Data bits, no parity, 2 Stop bit */
    UART_8N2 = 0x0E, /**< 8 Data bits, no parity, 2 Stop bit */
    UART_5E1 = 0x20, /**< 5 Data bits, even parity, 1 Stop bit */
    UART_6E1 = 0x22, /**< 6 Data bits, even parity, 1 Stop bit */
    UART_7E1 = 0x24, /**< 7 Data bits, even parity, 1 Stop bit */
    UART_8E1 = 0x26, /**< 8 Data bits, even parity, 1 Stop bit */
    UART_5E2 = 0x28, /**< 5 Data bits, even parity, 2 Stop bit */
    UART_6E2 = 0x2A, /**< 6 Data bits, even parity, 2 Stop bit */
    UART_7E2 = 0x2C, /**< 7 Data bits, even parity, 2 Stop bit */
    UART_8E2 = 0x2E, /**< 8 Data bits, even parity, 2 Stop bit */
    UART_5O1 = 0x30, /**< 5 Data bits, odd parity, 1 Stop bit */
    UART_6O1 = 0x32, /**< 6 Data bits, odd parity, 1 Stop bit */
    UART_7O1 = 0x34, /**< 7 Data bits, odd parity, 1 Stop bit */
    UART_8O1 = 0x36, /**< 8 Data bits, odd parity, 1 Stop bit */
    UART_5O2 = 0x38, /**< 5 Data bits, odd parity, 2 Stop bit */
    UART_6O2 = 0x3A, /**< 6 Data bits, odd parity, 2 Stop bit */
    UART_7O2 = 0x3C, /**< 7 Data bits, odd parity, 2 Stop bit */
    UART_8O2 = 0x3E /**< 8 Data bits, odd parity, 2 Stop bit */
} uart_fmt_t;

/**
 * @brief Define UART operation mode
 */
typedef enum
{
    UART_TX = 0x08, /**< Transmitter */
    UART_RX = 0x10  /**< Receiver */
} uart_mode_t;

//------------------------------------------------------------------------------
// Basic configuration
//------------------------------------------------------------------------------
/**
 * @brief UART initialization
 * @param baudrate UART baud rate.
 * @param format UART format, it is advised to use UART_8N1 as default.
 * @param mode UART operation mode: transmission or reception; can be OR'ed
 * @see uart_fmt_t
 * @see uart_mode_t
 */
void UART_init(uart_fmt_t format, uart_mode_t mode);

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

//------------------------------------------------------------------------------
// Advanced configuration
//------------------------------------------------------------------------------

void UART_reset(void);

/**
 * @brief Check whether the specific UART mode is enabled
 * @param mode Operation mode to check
 * @return `TRUE` if the operation mode is enabled, otherwise `FALSE`
 */
inline bool_t UART_is_enabled(uart_mode_t mode)
{
    return (BIT_is_set(UCSR0B, mode));
}

/**
 * @brief Enable specific UART operation mode
 * @param mode Operation mode to enable
 */
inline void UART_enable(uart_mode_t mode)
{
    BIT_set(UCSR0B, mode);
}

/**
 * @brief Disable specific UART operation mode
 * @param mode Operation mode to disable
 */
inline void UART_disable(uart_mode_t mode)
{
    BIT_clear(UCSR0B, mode);
}

/**
 * @brief Return the frame format
 * @return UART frame format
 * @see uart_fmt_t
 */
inline uart_fmt_t UART_get_format(void)
{
    return ((uart_fmt_t)UCSR0C);
}

/**
 * @brief Set UART frame format
 * @param format Frame format
 */
inline void UART_set_format(uart_fmt_t format)
{
    UCSR0C = (byte_t)format;
}

/**
 * @brief Enable UART interrupt
 * @param mode Operation mode interrupt to enable
 */
inline void UART_enable_interrupt(uart_mode_t mode)
{
    BIT_set(UCSR0B, (mode << 3));
}

/**
 * @brief Disable UART interrupt
 * @param mode Operation mode interrupt to disable
 */
inline void UART_disable_interrupt(uart_mode_t mode)
{
    BIT_clear(UCSR0B, (mode << 3));
}

/**
 * @brief Check whether receive is complete
 * @return `TRUE` if the receive is complete, otherwise `FALSE`
 */
inline bool_t UART_is_rx_complete(void)
{
    return (BIT_is_set(UCSR0A, BIT(RXC0)));
}

/**
 * @brief Check whether transmit is complete
 * @return `TRUE` if the transmit is complete, otherwise `FALSE`
 */
inline bool_t UART_is_tx_complete(void)
{
    return (BIT_is_set(UCSR0A, BIT(TXC0)));
}

/**
 * @brief Check whether a data overrun is detected
 * @return `TRUE` if a data overrun is detected, otherwise `FALSE`
 * @note A data overrun occurs when the receive buffer is full
 */
inline bool_t UART_has_overrun(void)
{
    return (BIT_is_set(UCSR0A, BIT(DOR0)));
}

/**
 * @brief Check whether a frame error has occured
 * @return `TRUE` if a frame error has occured, otherwise `FALSE`
 * @note A frame error occurs when the first stop bit of the next character in
 * the recieve buffer is `0`
 */
inline bool_t UART_has_frame_error(void)
{
    return (BIT_is_set(UCSR0A, BIT(FE0)));
}

/**
 * @brief Check whether a parity error has occured
 * @return `TRUE` if a parity error has occured, otherwise `FALSE`
 * @note A parity error occurs when
 */
inline bool_t UART_has_parity_error(void)
{
    return (BIT_is_set(UCSR0B, BIT(UPE0)));
}

/**
 * @brief Check whether the UART is ready
 * @return `TRUE` if the UART is ready, otherwise `FALSE`
 */
inline bool_t UART_is_ready(void)
{
    return (BIT_is_set(UCSR0A, BIT(UDRE0)));
}

/**
 * @brief Flush the receiver buffer
 */
void UART_flush(void);

#endif // VEMAR_UART_H

/**
 * @file uart.h
 * @brief UART utility functions
 * @author Christian Hugon <chriss.hugon@gmail.com>
 * @version 1.0.0
 */

/**
 * @page UART
 * @brief UART module
 * @section sec_uart_init Initialization
 * @par
 * The UART has to be initialized before any communication can take place.
 */
