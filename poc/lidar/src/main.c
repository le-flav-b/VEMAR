#include "main.h"
#include "uart.h"

int main()
{
	// Set OC1A (PB1, Arduino pin 9) as output
    DDRB |= (1 << DDB1);
    // Fast PWM, mode 14 (WGM13:0 = 1110)
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // prescaler = 1
    // TOP value for 10 kHz
    ICR1 = 1599;
    // 85% duty cycle
    OCR1A = 1360;

	uart_init();
	for (int i = 0; i < 300; i++)
	{
		uart_tx_hexa(uart_rx());
		uart_tx(' ');
	}

	while (1);
}
