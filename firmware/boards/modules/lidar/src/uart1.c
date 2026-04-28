#include "uart1.h"

#define UBRR_VALUE ((int)(F_CPU / (8.0 * UART1_BAUD) - .5))

/*
Init UART1 with :
- receiver
- transmitter
- 8N1 frame format
*/
void uart1_init()
{
	UBRR1H = (UBRR_VALUE >> 8); UBRR1L = UBRR_VALUE; // Set baud rate
    SET_MSK(UCSR1A, (1<<U2X1)); // Enable double speed
	SET_MSK(UCSR1B, (1<<RXEN1) | (1<<TXEN1)); // Enable receiver and transmitter
	SET_MSK(UCSR1C, (1<<UCSZ11) | (1<<UCSZ10)); // Set frame format 8N1
}

/*
Wait for a byte from UART1 and return it
Block the programme till a byte is received
*/
uint8_t uart1_rx()
{
	while (!GET_VALUE(UCSR1A, RXC1)); // Wait for data to be received
	uint8_t c = UDR1; // Get received data from buffer
	if (c == 0x7F) return '\b'; // Rectify 'backspace' transmission problem
	if (c == '\r') return '\n'; // Rectify 'new line' transmission problem
	return c; // Return received data from buffer
}

/*
Check for non-ascii key press
Supported keys :
- arrows
*/
static uint8_t _special_sequence(const uint8_t c)
{
	static uint8_t seq = 0;
	if (c == 0x1b) return seq = 1; // ESC key (beginning of a sequence)
	if (!seq) return 0; // Not a special sequence
	if (seq == 1 && c == '[') return seq++; // Still in a special sequence
	if (seq == 1) return seq = 0; // Not anymore in a special sequence
	return (seq = 0, c == 'A' || c == 'B' || c == 'C' || c == 'D'); // Reset the sequence
}

/*
Fill 'line' with the string received from UART1
Does not put the '\n' in 'line'
Return 1 if the input is larger than 'max_size', 0 otherwise
If 'max_size' is greater than allocated space of 'line' it can segfault
Block the programme till a '\n' is received
*/
uint8_t uart1_getline(char *line, const uint8_t max_size)
{
	uint8_t len = 0;
	uint8_t c;

	while (1)
	{
		c = uart1_rx();
		if (_special_sequence(c)) continue; // Do nothing if a supported special key is pressed
		if (c == '\b' && !len) continue; // Do nothing if 'backspace' but nothing to erease
		uart1_tx(c); // Show what we are typing
		if (c == '\n' && len < max_size) return (line[len] = '\0'); // 'line' set, return 0
		if (c == '\n') return 1; // Line greater than 'max_size', return 1
		if (c == '\b') { len--; continue; }; // Manage backspace
		if (len < max_size - 1) line[len] = c;
		len++;
	}
}

/*
Send the byte to UART1
*/
void uart1_tx(const uint8_t byte)
{
	static uint8_t rec = 0;
	while (!GET_VALUE(UCSR1A, UDRE1)); // Wait for empty transmit buffer
	UDR1 = byte; // Put data into buffer, sends the data
	if (byte == '\b' && !rec) { uart1_tx(' '); rec = 1; uart1_tx('\b'); } // Rectify 'backspace' transmission problem
	else if (byte == '\n') uart1_tx('\r'); // Rectify 'new line' transmission problem
	rec = 0;
}

/*
Send the hexa value of 'c' to UART1
*/
void uart1_tx_hexa(const uint8_t byte)
{
    const char hex[] = "0123456789ABCDEF";
    uart1_tx(hex[(byte >> 4) & 0x0F]);
    uart1_tx(hex[byte & 0x0F]);
}

/*
Send the string 'str' to UART1
*/
void uart1_printstr(const char *str)
{
	while (*str)
		uart1_tx(*str++);
}
