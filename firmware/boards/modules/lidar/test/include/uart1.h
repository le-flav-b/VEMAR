#ifndef UART_1_H
# define UART_1_H

void uart_init_1();
char uart_rx_1();
uint8_t uart_getline_1(char *line, const unsigned int max_size);
void uart_tx_1(const char c);
void uart_printstr_1(const char *str);

#endif
