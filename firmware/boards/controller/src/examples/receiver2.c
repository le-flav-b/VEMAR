#include "serial.h"
#include "radio.h"

#define PIN_CE PIN_PD6
#define PIN_CSN PIN_PD7
#define PIPE1 "11111"
#define PIPE2 "22222"

const byte_t addr[][5] = {PIPE1, PIPE2};
const byte_t wbuff[32] = "pong!!";
byte_t rbuff[32];
bool_t mode_tx;
bool_t label;
unsigned count;

static void send(void)
{
    if (RADIO_write(wbuff, 32))
    {
        SERIAL_println(str, "Successfully sent to Transmitter");
        mode_tx = 0;
        label = 0;
    }
    else
    {
        SERIAL_println(str, "Failed to send to Transmitter");
        ++count;
        if (count > 10)
        {
            count = 0;
            mode_tx = 0;
            label = 0;
        }
    }
    delay(1000);
}

static void receive(void)
{
    if (RADIO_read(rbuff, 32))
    {
        SERIAL_print(str, "From Transmitter: ");
        SERIAL_println(str, (char *)rbuff);
        mode_tx = 1;
        rbuff[0] = 0;
        label = 0;
    }
}

void setup(void)
{
    SERIAL_init();
    SERIAL_println(str, "==> Receiver");
    RADIO_init(PIN_CE, PIN_CSN);
    RADIO_set_address_rx(1, addr[0]);
    RADIO_set_address_tx(addr[1]);
    RADIO_debug();
    mode_tx = 0;
}

void loop(void)
{
    if (mode_tx)
    {
        if (!label)
        {
            label = 1;
            SERIAL_println(str, ">>> TX mode");
        }
        send();
    }
    else
    {
        if (!label)
        {
            label = 1;
            SERIAL_println(str, ">>> RX mode");
        }
        receive();
    }
}
