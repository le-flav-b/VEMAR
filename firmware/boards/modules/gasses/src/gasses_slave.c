#include "gasses.h"

static volatile struct i2cMessage msg = {0};
static volatile bool needs_fill = false;

// CO2 (MH-Z1911A): updated by UART RX interrupt
static volatile uint8_t  co2_buf[CO2_RESPONSE_LEN] = {0};
static volatile uint8_t  co2_idx = 0;
static volatile uint16_t co2_ppm = 0;
static volatile uint8_t  co2_temp_raw = 0;  // response[4]; subtract CO2_TEMP_OFFSET for °C
static volatile bool     co2_valid = false;  // true once a CRC-passing packet is received
static volatile bool     co2_rx_seen = false;
static volatile bool     co2_frame_seen = false;
static volatile bool     co2_uart_error = false;
static volatile bool     co2_rx_edge_seen = false;
static volatile bool     co2_cmd_sent = false;

// Preheat: count main-loop iterations (~1s each); sensor ready after CO2_PREHEAT_MS
static volatile uint16_t uptime_s = 0;
static volatile bool     co2_preheating = true;

// ─── UART (MH-Z1911A CO2 sensor) ────────────────────────────────────────────

static void uart_send_byte(uint8_t byte) {
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = byte;
}

static void uart_flush_rx(void) {
    while (USART0.STATUS & USART_RXCIF_bm) {
        (void)USART0.RXDATAH;
        (void)USART0.RXDATAL;
    }
}

static void co2_uart_prepare_request(void) {
    uint8_t sreg = SREG;
    cli();
    co2_idx = 0;
    uart_flush_rx();
    SREG = sreg;
}

static void co2_process_rx_byte(uint8_t status, uint8_t byte) {
    // Drop errored bytes and resync on next packet start.
    if (status & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm)) {
        co2_uart_error = true;
        co2_idx = 0;
        return;
    }

    co2_rx_seen = true;

    if (co2_idx == 0 && byte != 0xFF) return;
    if (co2_idx == 1 && byte != 0x86) { co2_idx = 0; return; }

    co2_buf[co2_idx++] = byte;

    if (co2_idx == CO2_RESPONSE_LEN) {
        uint8_t crc = 0;
        for (uint8_t i = 1; i < 8; i++) crc += co2_buf[i];
        crc = (~crc) + 1;
        co2_frame_seen = true;

        if (crc == co2_buf[8]) {
            co2_ppm      = ((uint16_t)co2_buf[2] << 8) | co2_buf[3];
            co2_temp_raw = co2_buf[4]; // caller subtracts CO2_TEMP_OFFSET (44) for °C
            co2_valid    = true;
        } else {
            co2_uart_error = true;
        }
        co2_idx = 0;
    }
}

static void co2_poll_uart_ms(uint16_t timeout_ms) {
    for (uint16_t ms = 0; ms < timeout_ms; ms++) {
        while (USART0.STATUS & USART_RXCIF_bm) {
            uint8_t status = USART0.RXDATAH;
            uint8_t byte = USART0.RXDATAL;
            co2_process_rx_byte(status, byte);
        }
        _delay_ms(1);
    }
}

static void send_co2_read_cmd(void) {
    // MH-Z1911A read CO2 command: FF 01 86 00 00 00 00 00 79
    const uint8_t cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    for (uint8_t i = 0; i < 9; i++) {
        uart_send_byte(cmd[i]);
    }
    co2_cmd_sent = true;
}

ISR(USART0_RXC_vect) {
    uint8_t status = USART0.RXDATAH;
    uint8_t byte = USART0.RXDATAL;
    co2_process_rx_byte(status, byte);
}

ISR(PORTA_PORT_vect) {
    uint8_t flags = PORTA.INTFLAGS;
    if (flags & PIN2_bm) {
        co2_rx_edge_seen = true;
        PORTA.INTFLAGS = PIN2_bm;
    }
}

// ─── ADC (MICS-6814 + O2 sensor) ────────────────────────────────────────────

static uint16_t adc_read(uint8_t muxpos) {
    ADC0.MUXPOS = muxpos;
    ADC0.COMMAND = ADC_STCONV_bm;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    return ADC0.RES;
}

// ─── I2C slave ───────────────────────────────────────────────────────────────

ISR(TWI0_TWIS_vect) {
    if (TWI0.SSTATUS & TWI_APIF_bm) {
        // Address or stop condition from master
        msg.current_idx = 0;
        if (msg.len == 0) {
            i2c_slave_nack();
        } else {
            i2c_slave_ack();
        }
    } else if (TWI0.SSTATUS & TWI_DIF_bm) {
        if (TWI0.SSTATUS & TWI_DIR_bm) {
            // Master is reading from us
            if (msg.current_idx < msg.len) {
                i2c_slave_transmit(msg.buffer[msg.current_idx++]);
            } else {
                i2c_slave_ack();
                needs_fill = true;
            }
        }
    }
}

// ─── Message fill ────────────────────────────────────────────────────────────

static void fill_msg(void) {
    // ADC reads are blocking/polling — do these with interrupts enabled
    uint16_t co_val  = adc_read(ADC_CO);
    uint16_t nh3_val = adc_read(ADC_NH3);
    uint16_t no2_val = adc_read(ADC_NO2);
    uint16_t o2_val  = adc_read(ADC_O2);

    uint8_t sreg = SREG;
    cli(); // Protect access to ISR-updated variables and msg struct

    uint16_t co2_val  = co2_ppm;
    uint8_t  temp_raw = co2_temp_raw;
    // STATUS byte:
    // bit 0 = co2_valid, bit 1 = co2_preheating,
    // bit 2 = co2_rx_seen, bit 3 = co2_frame_seen,
    // bit 4 = co2_uart_error, bit 5 = co2_rx_edge_seen,
    // bit 6 = co2_cmd_sent
    uint8_t  status   =
        (co2_valid ? 0x01 : 0x00) |
        (co2_preheating ? 0x02 : 0x00) |
        (co2_rx_seen ? 0x04 : 0x00) |
        (co2_frame_seen ? 0x08 : 0x00) |
        (co2_uart_error ? 0x10 : 0x00) |
        (co2_rx_edge_seen ? 0x20 : 0x00) |
        (co2_cmd_sent ? 0x40 : 0x00);
    uint16_t len      = GAS_PACKET_SIZE;

    // Packet layout (12 bytes payload + 2 bytes length header = 14 total):
    // [len_hi][len_lo][CO2_H][CO2_L][CO_H][CO_L][NH3_H][NH3_L][NO2_H][NO2_L][O2_H][O2_L][TEMP][STATUS]
    // TEMP   : response[4] from sensor — subtract CO2_TEMP_OFFSET (44) on master for °C
    // STATUS : see bit definitions above
    msg.buffer[0]  = (uint8_t)(len >> 8);
    msg.buffer[1]  = (uint8_t)(len & 0xFF);
    msg.buffer[2]  = (uint8_t)(co2_val >> 8);
    msg.buffer[3]  = (uint8_t)(co2_val & 0xFF);
    msg.buffer[4]  = (uint8_t)(co_val >> 8);
    msg.buffer[5]  = (uint8_t)(co_val & 0xFF);
    msg.buffer[6]  = (uint8_t)(nh3_val >> 8);
    msg.buffer[7]  = (uint8_t)(nh3_val & 0xFF);
    msg.buffer[8]  = (uint8_t)(no2_val >> 8);
    msg.buffer[9]  = (uint8_t)(no2_val & 0xFF);
    msg.buffer[10] = (uint8_t)(o2_val >> 8);
    msg.buffer[11] = (uint8_t)(o2_val & 0xFF);
    msg.buffer[12] = temp_raw;
    msg.buffer[13] = status;

    msg.current_idx = 0;
    msg.len = len + 2; // 2-byte length header + payload
    needs_fill = false;

    SREG = sreg;
}

// ─── Init ────────────────────────────────────────────────────────────────────

void uart_init(void) {
    // 9600 baud, 8N1 @ 20MHz system clock.
    // This module is built/flashed for 20MHz (see Makefile F_CPU).
    const uint32_t clk_hz = 20000000UL;

#if defined(PORTMUX_USART0_gm)
    PORTMUX.USARTROUTEA = (PORTMUX.USARTROUTEA & ~PORTMUX_USART0_gm) | PORTMUX_USART0_DEFAULT_gc;
#endif
    USART0.BAUD = (uint16_t)((64UL * clk_hz) / (16UL * 9600));
    USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc |
                   USART_PMODE_DISABLED_gc |
                   USART_SBMODE_1BIT_gc |
                   USART_CHSIZE_8BIT_gc;
    PORTA.DIRSET = PIN1_bm; // PA1 = AT_GAS_TX
    PORTA.DIRCLR = PIN2_bm; // PA2 = AT_GAS_RX
    PORTA.PIN2CTRL = PORT_ISC_BOTHEDGES_gc;
    PORTA.INTFLAGS = PIN2_bm;
    USART0.CTRLA = USART_RXCIE_bm;
    USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_RXMODE_NORMAL_gc;
}

void adc_init(void) {
    PORTA.DIRCLR = PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm; // PA4-PA7 as inputs
    ADC0.CTRLB = ADC_SAMPNUM_ACC1_gc;
    ADC0.CTRLC = ADC_PRESC_DIV64_gc | ADC_REFSEL_VDDREF_gc; // 20MHz/64 = ~312kHz, VDD ref
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;
}

void slave_init(void) {
    PORTB.DIRSET = PIN2_bm; // PB2 = STATUS_LED output
    i2c_init_slave(SLAVE_ADDR);
    uart_init();
    adc_init();
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main(void) {
    slave_init();

    _delay_ms(100);
    fill_msg(); // Populate buffer before master can request data

    // Enable 8s watchdog
    CCP = CCP_IOREG_gc;
    WDT.CTRLA = WDT_PERIOD_8KCLK_gc;

    sei();

    while (1) {
        // Continuous command mode: always request a frame and process replies.
        co2_uart_prepare_request();
        send_co2_read_cmd();
        co2_poll_uart_ms(500);

        // Always refresh payload so diagnostic bits reflect current runtime state.
        fill_msg();

        wdt_reset();
        _delay_ms(500); // ~1Hz command cadence

        // Track uptime for preheat (CO2_PREHEAT_MS = 60s per MHZ1911A datasheet)
        if (co2_preheating) {
            if (++uptime_s >= (CO2_PREHEAT_MS / 1000)) {
                co2_preheating = false;
            }
        }
    }
    return 0;
}
