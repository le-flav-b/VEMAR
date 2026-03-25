#include "i2c.h"
#include <util/delay.h>

// #if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)

#define ADDR 0x09

static volatile bool needs_fill = false;
static void fill_msg(void);
static const char *fake_sensor(void);
// static void prime_tx_header(void);

volatile struct i2cMessage msg = {0};


ISR(TWI0_TWIS_vect) {
    // APIF - Address or Stop Interrupt Flag (master calls start)
    if (TWI0.SSTATUS & TWI_APIF_bm) {
        if (msg.len == 0) {
            i2c_slave_nack();
        } else {
            i2c_slave_ack();
        }
    }
    // Master reading
    else if (TWI0.SSTATUS & TWI_DIF_bm) {
        if (TWI0.SSTATUS & TWI_DIR_bm) {
            if (TWI0.SSTATUS & TWI_RXACK_bm) {
                needs_fill = true;
            } else if (msg.current_idx < msg.len) {
                i2c_slave_transmit(msg.buffer[msg.current_idx++]);
            } else {
                i2c_slave_nack();
                needs_fill = true;
            }
        }
    }
}


static void fill_msg(void) {
    cli();
    const char *sensor_data = fake_sensor();
    uint16_t len = strlen(sensor_data);
    uint16_t max_payload = I2C_BUFFER_SIZE - sizeof(uint16_t);
    if (len > max_payload) {
        len = max_payload;
    }

    msg.buffer[0] = (uint8_t)(len >> 8);
    msg.buffer[1] = (uint8_t)(len & 0xFF);
    for (uint16_t i = 0; i < len; i++) {
        msg.buffer[sizeof(uint16_t) + i] = (uint8_t)sensor_data[i];
    }

    msg.current_idx = 0;
    msg.len = len + 2;
    needs_fill = false;
    sei();
}

static const char *fake_sensor(void) {
    return "success";
}

// static void prime_tx_header(void) {
//     msg.current_idx = 0;
//     if (msg.len > 0) {
//         TWI0.SDATA = msg.buffer[msg.current_idx++];
//     }
// }

void init(void) {
    i2c_init_slave(ADDR);
    sei();
}

int main(void) {
    fill_msg();
    init();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while (1) {
        sleep_mode();
        if (needs_fill) {
            fill_msg();
        }
    }
    return 0;
}

// #endif