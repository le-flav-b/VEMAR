#include "uart.h"

#define UART_MASK_INTERRUPT 0xE0

void UART_init(unsigned long baudrate, uart_format_t format, uart_mode_t mode)
{
	UART_set_baudrate(baudrate);
	UART_set_format(format);
	UART_enable_double_speed();

	UCSR0B = (byte_t)mode; // enable transmitter and/or receiver
}

void UART_transmit(byte_t data)
{
    WAIT_UNTIL(UART_is_data_empty());
    UDR0 = data;
}

byte_t UART_receive(void)
{
    WAIT_UNTIL(UART_is_rx_complete());
    return UDR0;
}

void UART_flush(void)
{
    byte_t dummy;

    while (UART_is_rx_complete())
    {
        dummy = UDR0;
        (void)dummy;
    } // while RXC0 flag is set
}

void UART_enable_interrupt(uart_interrupt_t interrupt)
{
	BIT_write(UCSR0B, interrupt, 0xE0);
}

extern inline void UART_set_baudrate(unsigned long baudrate);
extern inline void UART_set_format(uart_format_t format);
extern inline void UART_enable_double_speed(void);

extern inline void UART_enable_transmitter(void);
extern inline void UART_disable_transmitter(void);

extern inline void UART_enable_receiver(void);
extern inline void UART_disable_receiver(void);

extern inline bool_t UART_is_rx_complete(void);
extern inline bool_t UART_is_tx_complete(void);
extern inline bool_t UART_has_frame_error(void);

extern inline void UART_enable_rxci(void);
extern inline void UART_disable_rxci(void);

extern inline void UART_enable_txci(void);
extern inline void UART_disable_txci(void);

extern inline bool_t UART_is_data_empty(void);
