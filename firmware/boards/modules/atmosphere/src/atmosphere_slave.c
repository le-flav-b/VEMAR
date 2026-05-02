#include "atmosphere.h"

static volatile bool needs_fill = true;
volatile struct i2cMessage msg = {0};
volatile uint8_t pm_data_finished[PM_PACKET_SIZE] = {0};
// volatile uint8_t bme_data[BME_PACKET_SIZE] = {0};  /* master reads BME280 directly */
static volatile uint16_t sds_valid_frames = 0;
static volatile uint16_t sds_rx_bytes = 0;

#define SDS011_PACKET_LEN 10
static volatile uint8_t sds_rx_buf[SDS011_PACKET_LEN];
static volatile uint8_t sds_rx_idx = 0;

// static struct bme_calib calib = {0};  /* master reads BME280 directly */

#define BME280_ADDR           0x76
#define BME280_REG_RESET      0xE0
#define BME280_REG_CTRL_HUM   0xF2
#define BME280_REG_CTRL_MEAS  0xF4
#define BME280_REG_CONFIG     0xF5

#define UART_DEBUG_PULSE 1
#define UART_DEBUG_EVERY_LOOPS 50

static void clock_init_20mhz(void) {
#if defined(__AVR_ATtiny1614__) || defined(__AVR_ATtiny412__)
    // Select high-frequency internal oscillator as system clock source.
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
    }

    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB = 0; // Disable prescaler
#endif
}

void toggle_led(void) {
    // toggle LED on PB2
    PORTB.OUTTGL = PIN2_bm;
}

static void uart_send_byte(uint8_t byte) {
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = byte;
}

static void send_sds011_cmd(const uint8_t cmd[19]) {
    for (uint8_t i = 0; i < 19; i++) {
        uart_send_byte(cmd[i]);
    }
}

void send_sds011_wakeup(void) {
    // AA B4 06 01 01 00*10 FF FF checksum AB
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x06, 0xAB
    };
    send_sds011_cmd(cmd);
}

void set_sds011_query_mode(void) {
    // AA B4 02 01 01*11 FF FF checksum AB
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x02, 0xAB
    };
    send_sds011_cmd(cmd);
}

void set_sds011_active_mode(void) {
    // AA B4 02 01 00*11 FF FF checksum AB - Active/continuous mode
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x01, 0xAB
    };
    send_sds011_cmd(cmd);
}

void uart_init(void) {
    // Bit rate: 9600, 8 data, no parity, 1 stop
    const uint32_t baud = 9600UL;
    const uint32_t clk_hz = 20000000UL;

    USART0.BAUD = (uint16_t)((64UL * clk_hz + (8UL * baud)) / (16UL * baud));
    USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc |
                   USART_PMODE_DISABLED_gc |
                   USART_SBMODE_1BIT_gc |
                   USART_CHSIZE_8BIT_gc;
    PORTA.OUTSET = PIN1_bm; // Set TX line high before enabling output
    PORTA.DIRSET = PIN1_bm; // TX
    PORTA.DIRCLR = PIN2_bm; // RX
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    USART0.CTRLA = USART_RXCIE_bm;
    USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_RXMODE_NORMAL_gc;
    PORTMUX.CTRLB |= PORTMUX_USART0_bm; // Route USART0 to alternate pins: TX=PA1, RX=PA2
}

ISR(USART0_RXC_vect) {
    uint8_t status = USART0.RXDATAH;
    uint8_t byte   = USART0.RXDATAL;

    sds_rx_bytes++;

    if (status & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm)) {
        sds_rx_idx = 0;
        return;
    }

    if (sds_rx_idx == 0 && byte != 0xAA) return;
    if (sds_rx_idx == 1 && byte != 0xC0) { sds_rx_idx = 0; return; }

    sds_rx_buf[sds_rx_idx++] = byte;

    if (sds_rx_idx == SDS011_PACKET_LEN) {
        sds_rx_idx = 0;
        if (sds_rx_buf[9] != 0xAB) return;

        uint8_t csum = 0;
        for (uint8_t i = 2; i <= 7; i++) csum += sds_rx_buf[i];
        if (csum != sds_rx_buf[8]) return;

        pm_data_finished[0] = sds_rx_buf[2]; // PM2.5 low
        pm_data_finished[1] = sds_rx_buf[3]; // PM2.5 high
        pm_data_finished[2] = sds_rx_buf[4]; // PM10 low
        pm_data_finished[3] = sds_rx_buf[5]; // PM10 high
        sds_valid_frames++;
        // toggle_led();
    }
}


ISR(TWI0_TWIS_vect) {
    // APIF - Address or Stop Interrupt Flag
    if (TWI0.SSTATUS & TWI_APIF_bm) {
        if (TWI0.SSTATUS & TWI_AP_bm) {
            // Address match
            msg.current_idx = 0;
            if (msg.len == 0) {
                i2c_slave_nack();
            } else {
                i2c_slave_ack();
            }
        } else {
            // STOP condition - release bus cleanly
            TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
        }
    }
    // Data interrupt
    else if (TWI0.SSTATUS & TWI_DIF_bm) {
        if (TWI0.SSTATUS & TWI_DIR_bm) {
            // DIR=1: master is reading (slave transmitting)
            // After first byte, RXACK reflects master's response to previous byte
            if (msg.current_idx > 0 && (TWI0.SSTATUS & TWI_RXACK_bm)) {
                // Master sent NACK - finish transaction
                TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
                needs_fill = true;
            } else if (msg.current_idx < msg.len) {
                i2c_slave_transmit(msg.buffer[msg.current_idx++]);
            } else {
                // Master requested beyond available bytes - end transaction
                TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
                needs_fill = true;
            }
        }
    }
}


static int8_t bme_write_reg(uint8_t reg, uint8_t value) {
    int8_t err = i2c_start((BME280_ADDR << 1), false);
    if (err) { i2c_stop(); return err; }
    err = i2c_write(reg);
    if (err) { i2c_stop(); return err; }
    err = i2c_write(value);
    i2c_stop();
    return err;
}

static void bme_i2c_bus_clear(void) {
#if defined(__AVR_ATtiny1614__) || defined(__AVR_ATtiny412__)
    // Disable TWI so we can manually toggle the bus lines.
    TWI0.MCTRLA &= ~TWI_ENABLE_bm;
    TWI0.SCTRLA &= ~TWI_ENABLE_bm;

    // Release SCL/SDA and enable weak pull-ups during recovery.
    PORTB.DIRCLR = PIN0_bm | PIN1_bm;
    PORTB.OUTSET = PIN0_bm | PIN1_bm;
    _delay_us(5);

    // If SDA is stuck low, clock SCL up to 9 times to free the bus.
    for (uint8_t i = 0; i < 9 && !(PORTB.IN & PIN1_bm); i++) {
        PORTB.OUTCLR = PIN0_bm;
        PORTB.DIRSET = PIN0_bm;  // Drive SCL low
        _delay_us(5);
        PORTB.DIRCLR = PIN0_bm;  // Release SCL high
        _delay_us(5);
    }

    // Generate a STOP condition: SDA low -> SDA high while SCL is high.
    PORTB.DIRCLR = PIN0_bm;     // Ensure SCL released high
    _delay_us(5);
    PORTB.OUTCLR = PIN1_bm;
    PORTB.DIRSET = PIN1_bm;     // Drive SDA low
    _delay_us(5);
    PORTB.DIRCLR = PIN1_bm;     // Release SDA high
    _delay_us(5);

    // Leave pins as inputs; disable pull-ups (external pull-ups in use).
    PORTB.OUTCLR = PIN0_bm | PIN1_bm;
    PORTB.DIRCLR = PIN0_bm | PIN1_bm;
#endif
}

static void bme_init(void) {
    _delay_ms(50);
    bme_i2c_bus_clear();
    i2c_switch_to_master();
    (void)bme_write_reg(BME280_REG_RESET, 0xB6);
    _delay_ms(10);
    (void)bme_write_reg(BME280_REG_CTRL_HUM,  0x01);
    (void)bme_write_reg(BME280_REG_CONFIG,     0xC0);
    (void)bme_write_reg(BME280_REG_CTRL_MEAS,  0x27);
    i2c_switch_to_slave();
}

static void fill_msg(void) {
    uint8_t sreg;

    sreg = SREG;
    cli();
    msg.len = 0;
    SREG = sreg;

    uint16_t len = PM_PACKET_SIZE + 2;

    sreg = SREG;
    cli();

    msg.buffer[0] = (uint8_t)(len >> 8);
    msg.buffer[1] = (uint8_t)(len & 0xFF);

    for (uint8_t i = 0; i < PM_PACKET_SIZE; i++) {
        msg.buffer[sizeof(uint16_t) + i] = pm_data_finished[i];
    }
    msg.buffer[sizeof(uint16_t) + PM_PACKET_SIZE]     = (uint8_t)(sds_valid_frames & 0xFF);
    msg.buffer[sizeof(uint16_t) + PM_PACKET_SIZE + 1] = (uint8_t)(sds_rx_bytes & 0xFF);

    msg.current_idx = 0;
    msg.len = len + 2;
    needs_fill = false;

    SREG = sreg;
}

void slave_init(void) {
    clock_init_20mhz();
    PORTB.DIRSET = PIN2_bm; // STATUS_LED
    PORTB.OUTCLR = PIN2_bm;
    PORTB.DIRCLR = PIN0_bm | PIN1_bm; // TWI0 default pins: SCL/SDA inputs
    i2c_init_slave(SLAVE_ADDR);
    uart_init();
}

int main(void) {
    slave_init();
    _delay_ms(1000); // Wait for sensors to power up
    bme_init();
    send_sds011_wakeup();
    set_sds011_active_mode();
    _delay_ms(3000);  // Give sensor time to stabilize and start sending real data

    fill_msg();
    sei();

    while (1) {
        if (needs_fill) {
            fill_msg();
        }
        _delay_ms(10);
    }
    return 0;
}