#include "serial.h"
#include "radio.h"

#define PIN_CE PIN_PB1
#define PIN_CSN PIN_PB2
#define PIPE1 "11111"
#define PIPE2 "22222"

const byte_t addr[][5] = {PIPE1, PIPE2};
const byte_t wbuff[32] = "pong!!";
byte_t rbuff[32];
bool_t mode_tx;

static void send(void)
{
    if (RADIO_write(wbuff, 32))
    {
        SERIAL_println(str, "Successfully sent to Transmitter");
        mode_tx = 0;
    }
    else
    {
        SERIAL_println(str, "Failed to send to Transmitter");
    }
}

static void receive(void)
{
    if (RADIO_read(rbuff, 32))
    {
        SERIAL_print(str, "From Transmitter: ");
        SERIAL_println(str, (char *)rbuff);
        mode_tx = 1;
    }
}

void setup(void)
{
    SERIAL_init();
    SERIAL_println(str, "==> Receiver");
    RADIO_init(PIN_CE, PIN_CSN);
    RADIO_set_address_rx(0, addr[0]);
    // RADIO_set_address_tx(addr[0]);
    // RADIO_set_address_rx(1, addr[1]);
    mode_tx = 0;
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
