#ifndef UART0_H
# define UART0_H

# include "utils.h"

# ifndef UART0_BAUD
#  define UART0_BAUD 115200
# endif

void uart0_init();
uint8_t uart0_rx();
uint8_t uart0_getline(char *line, const uint8_t max_size);
void uart0_tx(const uint8_t byte);
void uart0_tx_hexa(const uint8_t byte);
void uart0_printstr(const char *str);

#endif
