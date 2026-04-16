#ifndef VEMAR_CONFIG_H
#define VEMAR_CONFIG_H

#ifndef F_CPU
#error "F_CPU is not defined"
#endif // F_CPU

#ifndef BAUDRATE
#define BAUDRATE 115200
#endif // BAUDRATE

#if !defined(_DOXYGEN_)
/**
 * @brief Value of UBRR in normal speed mode
 */
#define _UART_UBRR_N \
    (((F_CPU) + 8UL * (BAUDRATE)) / (16UL * (BAUDRATE)) - 1UL)

/**
 * @brief Baud rate calculated from _UART_UBBR_N
 */
#define _UART_BAUD_N \
    (((F_CPU) + 8UL * ((_UART_UBRR_N) + 1UL)) / (16UL * ((_UART_UBRR_N) + 1UL)))

/**
 * @brief Value of UBRR in double speed mode
 */
#define _UART_UBRR_2 \
    (((F_CPU) + 4UL * (BAUDRATE)) / (8UL * (BAUDRATE)) - 1UL)

/**
 * @brief Baud rate calculated from _UART_UBBR_2X
 */
#define _UART_BAUD_2 \
    (((F_CPU) + 4UL * ((_UART_UBRR_2) + 1UL)) / (8UL * ((_UART_UBRR_2) + 1UL)))

/**
 * @brief Calculate the absolute value of the difference
 */
#define _UART_ERROR(target, actual) \
    ((target > actual) ? (target - actual) : (actual - target))

#if _UART_ERROR(BAUDRATE, _UART_BAUD_2) < _UART_ERROR(BAUDRATE, _UART_BAUD_N)
#define UART_ENABLE_2X
#define UART_REG_UBRR0H (_UART_UBRR_2 >> 8)
#define UART_REG_UBRR0L (_UART_UBRR_2 & 0xFF)
#else // enable double speed
#define UART_REG_UBRR0H (_UART_UBRR_N >> 8)
#define UART_REG_UBRR0L (_UART_UBRR_N & 0xFF)
#endif // normal speed

#endif

/**
 * @brief Debounce time in milliseconds
 */
#define DEBOUNCE_TIME 10

#endif // VEMAR_CONFIG_H
