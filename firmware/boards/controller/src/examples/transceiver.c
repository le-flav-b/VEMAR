#include "radio.h"
#include "serial.h"

#define PAYLOAD_SIZE 32

button_t button;

union payload
{
    byte_t data[PAYLOAD_SIZE];
    struct details
    {
        uint8_t id;
        uint8_t ack;
    } details;
};

union payload packet_tx;
union payload packet_rx;

void setup(void)
{
    SERIAL_init();
    RADIO_init(PIN_PD6, PIN_PD7);
    button = BUTTON_new(PIN_PC5, BUTTON_ONPRESS);
    packet_tx.details.ack = 1;
}

void loop(void)
{
    if (BUTTON_is_active(&button))
    {
        if (RADIO_write(packet_tx.data, PAYLOAD_SIZE))
        {
            SERIAL_print(str, "Transmitted packet #");
            SERIAL_println(uint, packet_tx.details.id);
            packet_tx.details.id += 1;
        }
        else
        {
            SERIAL_print(str, "Packet #");
            SERIAL_print(uint, packet_tx.details.id);
            SERIAL_println(str, " lost");
        }
    }
    if (RADIO_read(packet_rx.data, PAYLOAD_SIZE))
    {
        SERIAL_print(str, "Received packet #");
        SERIAL_println(uint, packet_rx.details.id);
        if (packet_rx.details.ack)
        {
            packet_rx.details.id = 0xFF - packet_rx.details.id;
            packet_rx.details.ack = 0;
            while (!RADIO_write(packet_rx.data, PAYLOAD_SIZE))
            {
                delay(50);
            }
        }
    }
}
