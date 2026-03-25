#include "i2c.h"
#include <util/delay.h>

#define SLAVE_ADDR 0x09

// Simple UART functions for debugging
void uart_init(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
    // ATtiny USART initialization
    USART0.BAUD = (uint16_t)((F_CPU * 64) / (16 * 9600));
    USART0.CTRLB = USART_TXEN_bm; // Enable TX
#elif defined(__AVR_ATmega328P__)
    // ATmega328P UART initialization (9600 baud)
    uint16_t ubrr = F_CPU / 16 / 9600 - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
#endif
}

void uart_putc(char c) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = c;
#elif defined(__AVR_ATmega328P__)
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
#endif
}

void uart_puts(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}

void uart_print_hex(uint8_t byte) {
    const char hex[] = "0123456789ABCDEF";
    uart_putc(hex[byte >> 4]);
    uart_putc(hex[byte & 0x0F]);
}

void uart_print_int(int8_t num) {
    if (num < 0) {
        uart_putc('-');
        num = -num;
    }
    if (num >= 10) {
        uart_print_int(num / 10);
    }
    uart_putc('0' + (num % 10));
}

int main(void) {
    uart_init();
    i2c_init();
    uint8_t buffer[I2C_BUFFER_SIZE] = {0};
    
    uart_puts("I2C Master initialized\r\n");
    
    while (1) {
        int8_t read_resp = i2c_read_packet(SLAVE_ADDR, &buffer);
        if (read_resp) {
            // Print error
            uart_puts("Error reading from slave: ");
            uart_print_int(read_resp);
            uart_puts("\r\n");
        } else {
            // Print buffer contents in hex
            uart_puts("Data received: ");
            for (uint8_t i = 0; i < I2C_BUFFER_SIZE; i++) {
                if (buffer[i] == 0) break; // Stop at null terminator
                uart_print_hex(buffer[i]);
                uart_putc(' ');
            }
            uart_puts("\r\n");
        }
        memset(buffer, 0, sizeof(buffer));
        // Wait for 2 seconds
        _delay_ms(2000);
    }
}