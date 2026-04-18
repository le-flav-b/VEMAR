#ifndef UART_H
# define UART_H

# include "utils.h"

# ifndef UART_BAUD
#  define UART_BAUD 115200
# endif

void uart_init();
uint8_t uart_rx();
uint8_t uart_getline(char *line, const uint8_t max_size);
void uart_tx(const uint8_t byte);
void uart_tx_hexa(const uint8_t byte);
void uart_printstr(const char *str);

#endif
