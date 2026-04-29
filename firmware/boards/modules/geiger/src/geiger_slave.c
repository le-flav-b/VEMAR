#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "i2c.h"

#define GEIGER_ADDR  0x28
#define LED_PIN      PIN2_bm   /* PB2 - STATUS_LED */
#define INT_PIN      PIN6_bm   /* PA6 - AT_INT     */

static volatile uint32_t pulse_count = 0;
static volatile bool     needs_fill  = false;

static volatile struct i2cMessage msg = {0};

/* Pack current count into the I2C transmit buffer.
 * Wire format: [len_hi][len_lo][count_b3][count_b2][count_b1][count_b0]
 * Called from main with interrupts disabled. */
static void fill_msg(void) {
    cli();
    uint32_t count    = pulse_count;
    uint16_t payload  = sizeof(uint32_t);

    msg.buffer[0] = (uint8_t)(payload >> 8);
    msg.buffer[1] = (uint8_t)(payload & 0xFF);
    msg.buffer[2] = (uint8_t)(count >> 24);
    msg.buffer[3] = (uint8_t)(count >> 16);
    msg.buffer[4] = (uint8_t)(count >> 8);
    msg.buffer[5] = (uint8_t)(count & 0xFF);
    msg.len         = 6;
    msg.current_idx = 0;
    needs_fill      = false;
    sei();
}

ISR(TWI0_TWIS_vect) {
    if (TWI0.SSTATUS & TWI_APIF_bm) {
        msg.current_idx = 0;
        if (msg.len == 0) {
            i2c_slave_nack();
        } else {
            i2c_slave_ack();
        }
    } else if (TWI0.SSTATUS & TWI_DIF_bm) {
        if (TWI0.SSTATUS & TWI_DIR_bm) {
            if (msg.current_idx < msg.len) {
                i2c_slave_transmit(msg.buffer[msg.current_idx++]);
            } else {
                i2c_slave_ack();
                needs_fill = true;
            }
        }
    }
}

ISR(PORTA_PORT_vect) {
    if (PORTA.INTFLAGS & INT_PIN) {
        pulse_count++;
        PORTB.OUTTGL    = LED_PIN;
        PORTA.INTFLAGS  = INT_PIN;
    }
}

int main(void) {
    PORTB.DIRSET = LED_PIN;
    PORTB.OUTCLR = LED_PIN;

    /* startup blink — confirms LED circuit works before main loop */
    for (uint8_t i = 0; i < 6; i++) {
        PORTB.OUTTGL = LED_PIN;
        _delay_ms(200);
    }

    PORTA.DIRCLR   = INT_PIN;
    /* BOTHEDGES catches the pulse regardless of polarity for bringup.
     * Change to PORT_ISC_FALLING_gc once the correct edge is confirmed. */
    PORTA.PIN6CTRL = PORT_ISC_BOTHEDGES_gc;

    fill_msg();
    i2c_init_slave(GEIGER_ADDR);
    sei();

    set_sleep_mode(SLEEP_MODE_IDLE);
    while (1) {
        sleep_mode();
        if (needs_fill) {
            fill_msg();
        }
    }
    return 0;
}
