#include "i2c.h"

#define ADDR 0x09


volatile struct i2cMessage msg = {0};
volatile bool needs_update = true;


// change this for atmega
ISR(TWI0_TWIS_vect) {
    // APIF - Address or Stop Interrupt Flag (master calls start)
    if (TWI0.SSTATUS & TWI_APIF_bm) {
        if (msg.len == 0) {
            i2c_slave_nack();
        } else {
            i2c_slave_ack();
        }
    }
    // master reading
    else if (TWI0.SSTATUS & TWI_DIF_bm) {
        if (TWI0.SSTATUS & TWI_DIR_bm) {
            if (TWI0.SSTATUS & TWI_RXACK_bm) {
                clear_msg();
                needs_update = true;
            } else if (msg.current_idx < msg.len) {
                i2c_slave_transmit(msg.buffer[msg.current_idx++]);
            } else {
                clear_msg();
                needs_update = true;
            }
        }
    }
}


void fill_msg(void) {
    char * sensor_data = fake_sensor();
    uint16_t len = strlen(sensor_data);
    memcpy(msg.buffer, &len, sizeof(uint16_t));
    memcpy(msg.buffer + sizeof(uint16_t), sensor_data, len);
    msg.current_idx = 0;
    msg.len = len + 2;
}


void clear_msg(void) {
    memset(msg.buffer, 0, I2C_BUFFER_SIZE);
    msg.isFinished = false;
    msg.current_idx = 0;
    msg.len = 0;
}


char * fake_sensor(void) {
    return "success";
}


void init(void) {
    i2c_init_slave(ADDR);
    sei();
}


int main(void) {
    init();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while (1) {
        if (needs_update) {
            cli();
            fill_msg();
            needs_update = false;
            sei();
        }
        sleep_mode();
    }
    return 0;
}