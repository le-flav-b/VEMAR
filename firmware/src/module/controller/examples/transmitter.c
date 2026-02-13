#include <stdlib.h>

#include "serial.h"
#include "radio.h"
#include "nrf24l01.h"

#define PIN_CE PIN_PB1
#define PIN_CSN PIN_PB2
#define PIPE "11111"

const byte_t addr[] = PIPE;
const byte_t message[32] = "Hello World!";

void setup(void)
{
	SERIAL_init();
	SERIAL_println(str, ">> Transmitter");
	RADIO_init(PIN_CE, PIN_CSN);
	RADIO_set_address_tx(addr);

	// RADIO_debug();
}

void loop(void)
{
	// RADIO_ready_tx();
    // NRF24L01_enable();

	if (RADIO_write(message, 32))
	{
		SERIAL_println(str, "Success");
	}
	else
	{
		SERIAL_println(str, "Fail");
	}
	delay(1000);
}
