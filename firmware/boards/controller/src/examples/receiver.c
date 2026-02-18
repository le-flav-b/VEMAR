#include "serial.h"
#include "radio.h"

#define PIN_CE PIN_PD6
#define PIN_CSN PIN_PD7
#define PIPE "11111"


const byte_t addr[] = PIPE;
byte_t message[32];

void setup(void)
{
	SERIAL_init();
	SERIAL_println(str, "==> Receiver");
	RADIO_init(PIN_CE, PIN_CSN);
	RADIO_set_address_rx(0, addr);

	RADIO_debug();
}

void loop(void)
{
	if (RADIO_read(message, 32))
	{
		SERIAL_println(str, "Receive:");
		char *rec = (char *)message;
		SERIAL_println(str, rec);
	}
}
