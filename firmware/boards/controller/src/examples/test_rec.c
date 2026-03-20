#include "radio.h"
#include "serial.h"
#include "gpio.h"

led_t led;

typedef union payload
{
    int value;
    byte_t buffer[32];
} payload_t;

payload_t payload;

void setup(void)
{
    RADIO_init(PIN_PD6, PIN_PD7);
    SERIAL_init();
}

void loop(void)
{
    if (RADIO_read(payload.buffer, 32)) {
        SERIAL_println(int, payload.value);
        delay(1000);
    }
}
