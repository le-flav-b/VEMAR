#include "serial.h"
#include "radio.h"

#define PIN_CE PIN_PB1
#define PIN_CSN PIN_PB2
#define PIPE1 "11111"
#define PIPE2 "22222"

const byte_t addr[][5] = {PIPE1, PIPE2};
const byte_t wbuff[32] = "PING!";
byte_t rbuff[32];

bool_t mode_tx;

static void send(void)
{
    if (RADIO_write(wbuff, 32))
    {
        SERIAL_println(str, "Successfully sent to Receiver");
        mode_tx = 0;
    }
    else
    {
        SERIAL_println(str, "Failed to send to Receiver");
    }
    delay(1000);
}

static void receive(void)
{
    if (RADIO_read(rbuff, 32))
    {
        SERIAL_print(str, "From Receiver: ");
        SERIAL_println(str, rbuff);
        rbuff[0] = 0;
        mode_tx = 1;
    }
}

void setup(void)
{
    SERIAL_init();
    SERIAL_println(str, ">> Transmitter");
    RADIO_init(PIN_CE, PIN_CSN);
    RADIO_set_address_tx(addr[0]);
    RADIO_set_address_rx(1, addr[1]);
    mode_tx = 1;
    RADIO_debug();
}

void loop(void)
{
    if (mode_tx)
    {
        send();
    }
    else
    {
        receive();
    }
}
