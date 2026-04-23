#include "radio.h"
#include "serial.h"
#include "gpio.h"
#include "../../util/packet.h"

#define PIN_CE PIN_PD7
#define PIN_CSN PIN_PD6

int t = 0;
int h = 50;
int p = 0;

uint32_t count = 0;
packet_t payload;

void setup(void)
{
    RADIO_init(PIN_CE, PIN_CSN);
    SERIAL_init();
}

void loop(void)
{
    if (RADIO_read(payload.buffer, PACKET_SIZE))
    {
        if (PACKET_ID_CAR == payload.header.id)
        {
            SERIAL_println(str, ">> read: ");
            SERIAL_print(str, "LX: ");
            SERIAL_print(uint, payload.car.lx);
            SERIAL_print(str, "; LY: ");
            SERIAL_print(uint, payload.car.ly);
            SERIAL_print(str, "; LB: ");
            SERIAL_println(bool, payload.car.lb);
            SERIAL_print(str, "RX: ");
            SERIAL_print(uint, payload.car.rx);
            SERIAL_print(str, "; RY: ");
            SERIAL_print(uint, payload.car.ry);
            SERIAL_print(str, "; RB: ");
            SERIAL_println(bool, payload.car.rb);
            SERIAL_print(str, "Potentiometer: ");
            SERIAL_println(uint, payload.car.pot);
            SERIAL_println(str, "--------");
        }
    }
    if (++count > 10000)
    {
        count = 0;
        payload.header.id = PACKET_ID_ATM;
        payload.atmosphere.temperature = t;
        payload.atmosphere.humidity = h;
        payload.atmosphere.pressure = p;

        if (RADIO_write(payload.buffer, PACKET_SIZE))
        {
            SERIAL_print(str, "ID: ");
            SERIAL_print(int, payload.header.id);
            SERIAL_print(str, "; n: ");
            SERIAL_print(int, payload.atmosphere.pressure);
            SERIAL_print(str, "; t: ");
            SERIAL_print(int, payload.atmosphere.temperature);
            SERIAL_print(str, "; h: ");
            SERIAL_println(int, payload.atmosphere.humidity);

            t = (t + 12) % 1000;
            h = (h * 2 + 1) % 1000;
            p = (p + 1) % 1000;
        }
        // delay(1000);
    }
}
