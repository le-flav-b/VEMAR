#ifndef UART_H
# define UART_H

void uart_init();
char uart_rx();
uint8_t uart_getline(char *line, const unsigned int max_size);
void uart_tx(const char c);
void uart_tx_hexa(const char c);
void uart_printstr(const char *str);

#endif
