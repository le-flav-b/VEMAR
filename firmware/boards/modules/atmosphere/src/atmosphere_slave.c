#include "atmosphere.h"

static volatile bool needs_fill = true;
volatile struct i2cMessage msg = {0};
volatile uint8_t pm_data_finished[PM_PACKET_SIZE] = {0};
volatile uint8_t bme_data[BME_PACKET_SIZE] = {0};
static volatile uint16_t sds_valid_frames = 0;
static volatile uint16_t sds_rx_bytes = 0;

#define SDS011_PACKET_LEN 10
static volatile uint8_t sds_rx_buf[SDS011_PACKET_LEN];
static volatile uint8_t sds_rx_idx = 0;

static struct bme_calib calib = {0};

#define BME280_ADDR           0x76
#define BME280_REG_RESET      0xE0
#define BME280_REG_CTRL_HUM   0xF2
#define BME280_REG_STATUS     0xF3
#define BME280_REG_CTRL_MEAS  0xF4
#define BME280_REG_CONFIG     0xF5
#define BME280_REG_CALIB_00   0x88
#define BME280_REG_CALIB_26   0xE1
#define BME280_REG_DATA       0xF7

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

    // ATtiny1614: USART0 default pins PA1 (TX, pin 11), PA2 (RX, pin 12)
#if defined(PORTMUX_USART0_gm) && defined(PORTMUX_USART0_DEFAULT_gc)
    // PORTMUX.USARTROUTEA = (PORTMUX.USARTROUTEA & ~PORTMUX_USART0_gm) | PORTMUX_USART0_DEFAULT_gc;
    PORTMUX.USARTROUTEA = 0x00;
#elif defined(PORTMUX_USART0_bm)
    PORTMUX.CTRLB &= (uint8_t)~PORTMUX_USART0_bm;
#endif

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
    if (err != 0) {
        i2c_stop();
        return err;
    }

    err = i2c_write(reg);
    if (err != 0) {
        i2c_stop();
        return err;
    }

    err = i2c_write(value);
    i2c_stop();
    return err;
}

static int8_t bme_read_regs(uint8_t reg, uint8_t *data, uint8_t len) {
    int8_t err = i2c_start((BME280_ADDR << 1), false);
    if (err != 0) {
        i2c_stop();
        return err;
    }

    err = i2c_write(reg);
    i2c_stop();
    if (err != 0) {
        return err;
    }

    err = i2c_start((BME280_ADDR << 1) | 1, false);
    if (err != 0) {
        i2c_stop();
        return err;
    }

    for (uint8_t i = 0; i < len; i++) {
        int16_t rx = (i == (uint8_t)(len - 1)) ? i2c_read_nack() : i2c_read_ack();
        if (rx < 0) {
            i2c_stop();
            return (int8_t)rx;
        }
        data[i] = (uint8_t)rx;
    }

    i2c_stop();
    return 0;
}

static bool bme_wait_ready(uint16_t timeout_ms) {
    uint8_t status = 0;
    while (timeout_ms-- > 0) {
        if (bme_read_regs(BME280_REG_STATUS, &status, 1) == 0) {
            if ((status & 0x09) == 0) {
                return true;
            }
        }
        _delay_ms(1);
    }
    return false;
}

static void blink(uint8_t n, uint16_t on_ms, uint16_t off_ms) {
    for (uint8_t i = 0; i < n; i++) {
        PORTB.OUTSET = PIN2_bm;
        for (uint16_t j = 0; j < on_ms; j++) _delay_ms(1);
        PORTB.OUTCLR = PIN2_bm;
        for (uint16_t j = 0; j < off_ms; j++) _delay_ms(1);
    }
}

static void calibrate_bme(void) {
    uint8_t calib_data[28] = {0};
    uint8_t calib_tp[26] = {0};
    uint8_t calib_h[7] = {0};

    i2c_switch_to_master();

    // Verify chip ID — must read 0x60 from register 0xD0.
    // Blink count encodes the I2C error: 1=arblost 2=buserr 3=NACK(wrong addr) 5=timeout(bus stuck) 4=wrong chip ID
    uint8_t chip_id = 0;
    int8_t id_err = bme_read_regs(0xD0, &chip_id, 1);
    if (id_err != 0) {
        uint8_t n = (id_err == -3) ? 3 : (id_err == -5) ? 5 : (id_err == -1) ? 1 : 2;
        blink(n, 200, 200);
        blink(n, 200, 200); // repeat so it's easy to count
        i2c_switch_to_slave();
        return;
    }
    if (chip_id != 0x60) {
        blink(4, 200, 200); // 4 blinks = wrong chip ID (got something, not BME280)
        blink(4, 200, 200);
        i2c_switch_to_slave();
        return;
    }

    // Soft reset; sensor may NACK the data byte as it resets — expected.
    (void)bme_write_reg(BME280_REG_RESET, 0xB6);
    _delay_ms(10); // 10ms > datasheet 2ms startup time

    // config: t_standby=10ms (bits[7:5]=110), filter off → 0xC0
    // ctrl_hum must be written before ctrl_meas to take effect
    (void)bme_write_reg(BME280_REG_CTRL_HUM, 0x01);
    (void)bme_write_reg(BME280_REG_CONFIG, 0xC0);
    (void)bme_write_reg(BME280_REG_CTRL_MEAS, 0x27);

    int8_t err1 = bme_read_regs(BME280_REG_CALIB_00, calib_tp, sizeof(calib_tp));
    int8_t err2 = bme_read_regs(BME280_REG_CALIB_26, calib_h, sizeof(calib_h));

    if (err1 != 0 || err2 != 0) {
        // blink(3, 100, 100); // 3 fast blinks = calibration read failed
        i2c_switch_to_slave();
        return;
    }

    for (uint8_t i = 0; i < 24; i++) {
        calib_data[i] = calib_tp[i];
    }
    calib_data[24] = calib_tp[25];
    calib_data[25] = calib_h[0];
    calib_data[26] = calib_h[1];
    calib_data[27] = calib_h[2];

    load_bme280_calib(calib_data, &calib);
    calib.dig_H4 = ((int16_t)calib_h[3] << 4) | (calib_h[4] & 0x0F);
    calib.dig_H5 = ((int16_t)calib_h[5] << 4) | (calib_h[4] >> 4);
    calib.dig_H6 = (int8_t)calib_h[6];

    blink(1, 500, 0); // 1 long blink = calibration loaded OK

    i2c_switch_to_slave();
}

void read_bme(void) {
    uint8_t raw_bme_data[8] = {0};

    i2c_switch_to_master();
    if (!bme_wait_ready(50)) {
        i2c_switch_to_slave();
        return;
    }

    if (bme_read_regs(BME280_REG_DATA, raw_bme_data, sizeof(raw_bme_data)) == 0) {
        BME280_S32_t adc_P = (BME280_S32_t)(((uint32_t)raw_bme_data[0] << 12) |
                                            ((uint32_t)raw_bme_data[1] << 4) |
                                            ((uint32_t)raw_bme_data[2] >> 4));
        BME280_S32_t adc_T = (BME280_S32_t)(((uint32_t)raw_bme_data[3] << 12) |
                                            ((uint32_t)raw_bme_data[4] << 4) |
                                            ((uint32_t)raw_bme_data[5] >> 4));
        BME280_S32_t adc_H = (BME280_S32_t)(((uint32_t)raw_bme_data[6] << 8) |
                                             (uint32_t)raw_bme_data[7]);

        int32_t temp_comp = BME280_compensate_T_int32(adc_T, &calib);
        uint32_t pres_comp = BME280_compensate_P_int64(adc_P, &calib);
        uint32_t hum_comp = bme280_compensate_H_int32(adc_H, &calib);

        bme_data[0] = (uint8_t)((temp_comp >> 24) & 0xFF);
        bme_data[1] = (uint8_t)((temp_comp >> 16) & 0xFF);
        bme_data[2] = (uint8_t)((temp_comp >> 8) & 0xFF);
        bme_data[3] = (uint8_t)(temp_comp & 0xFF);

        bme_data[4] = (uint8_t)((pres_comp >> 24) & 0xFF);
        bme_data[5] = (uint8_t)((pres_comp >> 16) & 0xFF);
        bme_data[6] = (uint8_t)((pres_comp >> 8) & 0xFF);
        bme_data[7] = (uint8_t)(pres_comp & 0xFF);

        bme_data[8] = (uint8_t)((hum_comp >> 24) & 0xFF);
        bme_data[9] = (uint8_t)((hum_comp >> 16) & 0xFF);
        bme_data[10] = (uint8_t)((hum_comp >> 8) & 0xFF);
        bme_data[11] = (uint8_t)(hum_comp & 0xFF);
        // toggle_led();
    }

    i2c_switch_to_slave();
}

static void fill_msg(void) {
    uint8_t sreg;

    // Mark message unavailable so the ISR NACKs address matches during BME read.
    // msg.len is uint16_t but len <= 255 here so MSB is already 0; writing 0 to
    // the LSB is effectively atomic on AVR, but we guard it anyway.
    sreg = SREG;
    cli();
    msg.len = 0;
    SREG = sreg;

    // BME read runs with interrupts enabled — TWI slave ISR can fire and NACK.
    read_bme();

    uint16_t len = PM_PACKET_SIZE + BME_PACKET_SIZE + 2; // +2 debug bytes

    // Atomically install the new message.
    sreg = SREG;
    cli();

    msg.buffer[0] = (uint8_t)(len >> 8);
    msg.buffer[1] = (uint8_t)(len & 0xFF);

    for (uint8_t i = 0; i < PM_PACKET_SIZE; i++) {
        msg.buffer[sizeof(uint16_t) + i] = pm_data_finished[i];
    }
    for (uint8_t i = 0; i < BME_PACKET_SIZE; i++) {
        msg.buffer[sizeof(uint16_t) + PM_PACKET_SIZE + i] = bme_data[i];
    }
    msg.buffer[sizeof(uint16_t) + PM_PACKET_SIZE + BME_PACKET_SIZE]     = (uint8_t)(sds_valid_frames & 0xFF);
    msg.buffer[sizeof(uint16_t) + PM_PACKET_SIZE + BME_PACKET_SIZE + 1] = (uint8_t)(sds_rx_bytes & 0xFF);

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
    calibrate_bme();
    
    _delay_ms(8000);
    send_sds011_wakeup();
    _delay_ms(1000);
    set_sds011_active_mode();
    _delay_ms(3000);  // Give sensor time to stabilize and start sending real data

    read_bme();
    fill_msg();
    sei();

    while (1) {
        if (needs_fill) {
            read_bme();
            fill_msg();
        }
#if UART_DEBUG_PULSE
        static uint16_t uart_debug_ticks = 0;
        if (++uart_debug_ticks >= UART_DEBUG_EVERY_LOOPS) {
            uart_debug_ticks = 0;
            uart_send_byte(0x55);
        }
#endif
        // PORTA.OUTTGL = PIN1_bm;
        // _delay_ms(100);
        _delay_ms(10);
    }
    return 0;
}