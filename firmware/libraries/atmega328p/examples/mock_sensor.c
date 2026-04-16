#include "radio.h"
#include "serial.h"
#include "gpio.h"
#include "../main/packet.h"

packet_t payload;

int t = 0;
int h = 50;
int p = 0;

uint32_t count = 0;

void setup(void)
{
    RADIO_init(PIN_PD6, PIN_PD7);
    SERIAL_init();
}

void loop(void)
{
    if (RADIO_read(payload.data, PACKET_SIZE))
    {
        if (PACKET_ID_CAR == payload.header.id)
        {
            SERIAL_print(str, "read: ");
            SERIAL_println(uint, payload.car.velocity);
        }
    }
    if (++count > 10000)
    {
        count = 0;
        payload.header.id = PACKET_ID_ATM;
        payload.atmosphere.temperature = t;
        payload.atmosphere.humidity = h;
        payload.atmosphere.pressure = p;

        if (RADIO_write(payload.data, PACKET_SIZE))
        {
            // SERIAL_print(str, "ID: ");
            // SERIAL_print(int, payload.header.id);
            // SERIAL_print(str, "; n: ");
            // SERIAL_print(int, payload.sensor.pressure);
            // SERIAL_print(str, "; t: ");
            // SERIAL_print(int, payload.sensor.temperature);
            // SERIAL_print(str, "; h: ");
            // SERIAL_println(int, payload.sensor.humidity);

            t = (t + 12) % 1000;
            h = (h * 2 + 1) % 1000;
            p = (p + 1) % 1000;
        }
        // delay(1000);
    }
}
