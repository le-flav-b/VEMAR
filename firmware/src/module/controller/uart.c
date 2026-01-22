#include <avr/io.h>

#include "uart.h"

void UART_init(unsigned long baudrate, byte_t format)
{
    // unsigned int baud_ubrr = (unsigned int)(F_CPU / 8UL / baudrate - 1);
    UBRR0H = (byte_t)(baudrate >> 8); // 4 most significant bits
    UBRR0L = (byte_t)(baudrate);      // 8 least significant bits

    // UBRR0H = (byte_t)(baud_ubrr >> 8); // 4 most significant bits
    // UBRR0L = (byte_t)(baud_ubrr);      // 8 least significant bits
    UCSR0A = BIT(U2X0); // enable double speed

    UCSR0B = (BIT(RXEN0) | BIT(TXEN0)); // enable receiver and transmitter
    UCSR0C = format;                    // set serial configuration
}

void UART_transmit(byte_t data)
{
    // WAIT_UNTIL((UCSR0A & BIT(UDRE0))); // wait until data register is empty
    WAIT_UNTIL(UART_is_data_empty());
    UDR0 = data;
}

byte_t UART_receive(void)
{
    // WAIT_UNTIL((UCSR0A & BIT(RXC0))); // wait until data is received
    WAIT_UNTIL(UART_is_rx_complete());
    return UDR0;
}

void UART_flush(void)
{
    byte_t dummy;

    // while (BIT_read(UCSR0A, BIT(RXC0)))
    while (UART_is_rx_complete())
    {
        dummy = UDR0;
        (void)dummy;
    } // while RXC0 flag is set
}

void UART_print(const char *str)
{
    while ('\0' != *str)
    {
        UART_transmit(*str);
        ++str;
    } // while not null-terminating character
}

void UART_println(const char *str)
{
    UART_print(str);
    UART_transmit('\r');
    UART_transmit('\n');
}
