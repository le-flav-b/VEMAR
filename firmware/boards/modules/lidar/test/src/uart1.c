#include "main.h"
#include "uart1.h"


#define BAUD_1 115200
#define UBRR_VALUE_1 ((int)(F_CPU / (8.0 * BAUD_1) - .5))


/*
Init UART 1 with :
- receiver
- transmitter
- 8N1 frame format
*/
void uart_init_1()
{
	UBRR1H = (UBRR_VALUE_1 >> 8); UBRR1L = UBRR_VALUE_1; // Set baud rate
    SET_MSK(UCSR1A, (1<<U2X1)); // Enable double speed
	SET_MSK(UCSR1B, (1<<RXEN1) | (1<<TXEN1)); // Enable receiver and transmitter
	SET_MSK(UCSR1C, (1<<UCSZ11) | (1<<UCSZ10)); // Set frame format 8N1
}

/*
Wait for a character from UART1 and return it
Block the programme till a character is received
*/
char uart_rx_1()
{
	while (!GET_VALUE(UCSR1A, RXC1)); // Wait for data to be received
	char c = UDR1; // Get received data from buffer
	if (c == 0x7F) return '\b'; // Rectify 'backspace' transmission problem
	if (c == '\r') return '\n'; // Rectify 'new line' transmission problem
	return c; // Return received data from buffer
}

/*
Check for non-ascii key press
Supported keys :
- arrows
*/
uint8_t _special_sequence_1(const char c)
{
	static uint8_t seq = 0;
	if (c == 0x1b) return seq = 1; // ESC key (beginning of a sequence)
	if (!seq) return 0; // Not a special sequence
	if (seq == 1 && c == '[') return seq++; // Still in a special sequence
	if (seq == 1) return seq = 0; // Not anymore in a special sequence
	return (seq = 0, c == 'A' || c == 'B' || c == 'C' || c == 'D'); // Reset the sequence
}

/*
Fill 'line' with the string received from UART 1
Does not put the '\n' in 'line'
Return 1 if the input is larger than 'max_size', 0 otherwise
If 'max_size' is greater than allocated space of 'line' it can segfault
Block the programme till a '\n' is received
*/
uint8_t uart_getline_1(char *line, const unsigned int max_size)
{
	int len = 0;
	char c;

	while (1)
	{
		c = uart_rx_1();
		if (_special_sequence_1(c)) continue; // Do nothing if a supported special key is pressed
		if (c == '\b' && !len) continue; // Do nothing if 'backspace' but nothing to erease
		uart_tx_1(c); // Show what we are typing
		if (c == '\n' && len < max_size) return (line[len] = '\0'); // 'line' set, return 0
		if (c == '\n') return 1; // Line greater than 'max_size', return 1
		if (c == '\b') { len--; continue; }; // Manage backspace
		if (len < max_size - 1) line[len] = c;
		len++;
	}
}

/*
Send the character 'c' to UART 1
*/
void uart_tx_1(const char c)
{
	static uint8_t rec = 0;
	while (!GET_VALUE(UCSR1A, UDRE1)); // Wait for empty transmit buffer
	UDR1 = c; // Put data into buffer, sends the data
	if (c == '\b' && !rec) { uart_tx_1(' '); rec = 1; uart_tx_1('\b'); } // Rectify 'backspace' transmission problem
	else if (c == '\n') uart_tx_1('\r'); // Rectify 'new line' transmission problem
	rec = 0;
}

/*
Send the string 'str' to UART 1
*/
void uart_printstr_1(const char *str)
{
	while (*str)
		uart_tx_1(*str++);
}
