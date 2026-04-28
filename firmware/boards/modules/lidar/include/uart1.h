#ifndef UART1_H
# define UART1_H

# include "utils.h"

# ifndef UART1_BAUD
#  define UART1_BAUD 115200
# endif

void uart1_init();
uint8_t uart1_rx();
uint8_t uart1_getline(char *line, const uint8_t max_size);
void uart1_tx(const uint8_t byte);
void uart1_tx_hexa(const uint8_t byte);
void uart1_printstr(const char *str);

#endif
