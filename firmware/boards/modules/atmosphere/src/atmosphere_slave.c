#include "atmosphere.h"

static volatile bool needs_fill = true;
volatile struct i2cMessage msg = {0};
volatile uint8_t pm_data_unfinished[RAW_PM_PACKET_SIZE] = {0};
volatile uint8_t pm_data_finished[PM_PACKET_SIZE] = {0};
// volatile uint8_t bme_data[BME_PACKET_SIZE] = {0};
static volatile uint8_t uart_idx = 0;
static volatile uint16_t sds_valid_frames = 0;
static volatile uint16_t sds_checksum_fail_frames = 0;
static volatile uint16_t sds_uart_error_bytes = 0;
static volatile uint16_t sds_rx_bytes = 0;
static volatile uint16_t sds_aa_headers = 0;
static volatile uint16_t sds_c0_headers = 0;
static volatile uint16_t sds_c5_headers = 0;
static volatile uint16_t sds_query_sent = 0;
static volatile uint16_t sds_rx_edges = 0;
static volatile uint8_t reset_flags_snapshot = 0;
static uint8_t sds_query_cooldown = 0;
// static struct bme_calib calib = {0};  // BME280 calibration data

static void clock_init_20mhz(void) {
#if defined(__AVR_ATtiny1614__) || defined(__AVR_ATtiny412__)
    // Ensure runtime clock matches F_CPU used for UART/I2C timing math.
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
    // checksum = (0x06 + 0x01 + 0x01 + 0xFF + 0xFF) % 256 = 0x06
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x06, 0xAB
    };
    send_sds011_cmd(cmd);
}

void set_sds011_active_mode(void) {
    // AA B4 02 01 00*11 FF FF checksum AB
    // checksum = (0x02 + 0x01 + 0xFF + 0xFF) % 256 = 0x01
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x01, 0xAB
    };
    send_sds011_cmd(cmd);
}

void set_sds011_query_mode(void) {
    // AA B4 02 01 01*11 FF FF checksum AB
    // checksum = (0x02 + 0x01 + 0x01 + 0xFF + 0xFF) % 256 = 0x02
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x02, 0xAB
    };
    send_sds011_cmd(cmd);
}

void set_sds011_continuous_mode(void) {
    // AA B4 08 01 00*10 FF FF checksum AB
    // checksum = (0x08 + 0x01 + 0x00 + 0xFF + 0xFF) % 256 = 0x07
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x07, 0xAB
    };
    send_sds011_cmd(cmd);
}

void query_sds011_pm(void) {
    // AA B4 04 00*12 FF FF checksum AB
    // checksum = (0x04 + 0xFF + 0xFF) % 256 = 0x02
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x02, 0xAB
    };
    send_sds011_cmd(cmd);
    sds_query_sent++;
}

static void process_sds011_rx_byte(uint8_t status, uint8_t byte) {
    if (status & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm)) {
        sds_uart_error_bytes++;
        uart_idx = 0;
        return;
    }

    if (uart_idx == 0) {
        if (byte != 0xAA) {
            return;
        }
        sds_aa_headers++;
        pm_data_unfinished[uart_idx++] = byte;
        return;
    }

    // Second byte must be 0xC0 (PM data response ID); discard command responses.
    if (uart_idx == 1) {
        if (byte == 0xC0) {
            sds_c0_headers++;
            pm_data_unfinished[uart_idx++] = byte;
        } else if (byte == 0xC5) {
            sds_c5_headers++;
            uart_idx = 0;
        } else if (byte == 0xAA) {
            sds_aa_headers++;
            pm_data_unfinished[0] = 0xAA;
            uart_idx = 1;
        } else {
            uart_idx = 0;
        }
        return;
    }

    pm_data_unfinished[uart_idx++] = byte;

    if (uart_idx == RAW_PM_PACKET_SIZE) {
        uint8_t checksum = 0;
        for (uint8_t i = 2; i <= 7; i++) {
            checksum += pm_data_unfinished[i];
        }

        if (pm_data_unfinished[9] == 0xAB && checksum == pm_data_unfinished[8]) {
            for (uint8_t i = 2; i < 6; i++) {
                pm_data_finished[i - 2] = pm_data_unfinished[i];
            }
            sds_valid_frames++;
            toggle_led(); // Blink once per valid PM frame
        } else {
            sds_checksum_fail_frames++;
        }
        uart_idx = 0;
    }
}

static void poll_sds011_uart(void) {
    while (USART0.STATUS & USART_RXCIF_bm) {
        uint8_t status = USART0.RXDATAH;
        uint8_t byte = USART0.RXDATAL;
        sds_rx_bytes++;
        process_sds011_rx_byte(status, byte);
    }
}

static void poll_sds011_uart_ms(uint16_t duration_ms);

static void reset_sds_parser(void) {
    uart_idx = 0;
}

static void flush_sds011_uart_rx(void) {
    while (USART0.STATUS & USART_RXCIF_bm) {
        (void)USART0.RXDATAH;
        (void)USART0.RXDATAL;
    }
}

static void request_sds011_query_with_retry(void) {
    uint16_t valid_before = sds_valid_frames;

    // Reference libraries flush stale bytes before issuing a command.
    flush_sds011_uart_rx();
    reset_sds_parser();

    // Throttle to a single query transaction; avoid hammering the sensor.
    query_sds011_pm();
    poll_sds011_uart_ms(700);

    // If data is late, keep listening a bit longer without sending a second command.
    if (sds_valid_frames == valid_before) {
        poll_sds011_uart_ms(250);
    }
}

static void poll_sds011_uart_ms(uint16_t duration_ms) {
    for (uint16_t i = 0; i < duration_ms; i++) {
        poll_sds011_uart();
        _delay_ms(1);
    }
    poll_sds011_uart();
}

ISR(USART0_RXC_vect) {
    uint8_t status = USART0.RXDATAH;
    uint8_t byte = USART0.RXDATAL;
    process_sds011_rx_byte(status, byte);
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

void uart_init(void) {
    // Bit rate: 9600
    // Data bit: 8
    // Parity bit: NO
    // Stop bit: 1
    // Data Packet Freq: 1Hz

    // set board for 20Mhz
#if defined(PORTMUX_USART0_gm)
    PORTMUX.USARTROUTEA = (PORTMUX.USARTROUTEA & ~PORTMUX_USART0_gm) | PORTMUX_USART0_DEFAULT_gc;
#endif
    USART0.BAUD = (uint16_t)((64UL * F_CPU) / (16UL * 9600)); // tinyAVR formula
    USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc |
                   USART_PMODE_DISABLED_gc |
                   USART_SBMODE_1BIT_gc |
                   USART_CHSIZE_8BIT_gc;
    PORTA.DIRSET = PIN1_bm; // TX
    PORTA.DIRCLR = PIN2_bm; // RX
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm; // Keep RX high when line is idle/open
    USART0.CTRLA = 0; // Poll RX in main loop to avoid UART ISR interference with I2C
    USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_RXMODE_NORMAL_gc; // Enable TX/RX; RX IRQ is enabled in CTRLA
}


// void calibrate_bme(void) {
//     i2c_switch_to_master();
//     uint8_t calib_data[26] = {0};
//     int8_t err = -1;
//     for (uint8_t retry = 0; retry < 3; retry++) {
//         _delay_ms(10);
//         err = i2c_read_packet(0x76, calib_data);
//         if (err == 0) break;  // Success, exit retry loop
//     }
//     if (err != 0) {
//         // TODO: handle calibration failure (log error, halt, or use default)
//     }
//     load_bme280_calib(calib_data, &calib);
//     i2c_switch_to_slave();
// }


// void read_bme(void) {
//     i2c_switch_to_master();
//     uint8_t raw_bme_data[8] = {0};  // Temperature, Pressure, Humidity ADC data
//     int8_t err = -1;
//     for (uint8_t retry = 0; retry < 3; retry++) {
//         _delay_ms(10);
//         err = i2c_read_packet(0x76, raw_bme_data);
//         if (err == 0) break;  // Success, exit retry loop
//     }
//     if (err != 0) {
//         // TODO: handle persistent BME read failure
//     }
    
//     // Extract 20-bit ADC values from raw data
//     BME280_S32_t adc_T = (raw_bme_data[0] << 12) | (raw_bme_data[1] << 4) | (raw_bme_data[2] >> 4);
//     BME280_S32_t adc_P = (raw_bme_data[3] << 12) | (raw_bme_data[4] << 4) | (raw_bme_data[5] >> 4);
//     BME280_S32_t adc_H = (raw_bme_data[6] << 8) | raw_bme_data[7];
    
//     // Compensate and pack into message buffer as compensated values
//     int32_t temp_comp = BME280_compensate_T_int32(adc_T, &calib);
//     uint32_t pres_comp = BME280_compensate_P_int64(adc_P, &calib);
//     uint32_t hum_comp = bme280_compensate_H_int32(adc_H, &calib);
    
//     // Pack compensated values into bme_data (4 bytes each)
//     bme_data[0] = (temp_comp >> 24) & 0xFF;
//     bme_data[1] = (temp_comp >> 16) & 0xFF;
//     bme_data[2] = (temp_comp >> 8) & 0xFF;
//     bme_data[3] = temp_comp & 0xFF;
    
//     bme_data[4] = (pres_comp >> 24) & 0xFF;
//     bme_data[5] = (pres_comp >> 16) & 0xFF;
//     bme_data[6] = (pres_comp >> 8) & 0xFF;
//     bme_data[7] = pres_comp & 0xFF;
    
//     bme_data[8] = (hum_comp >> 24) & 0xFF;
//     bme_data[9] = (hum_comp >> 16) & 0xFF;
//     bme_data[10] = (hum_comp >> 8) & 0xFF;
//     bme_data[11] = hum_comp & 0xFF;
    
//     i2c_switch_to_slave();
// }

static void fill_msg(void) {
    uint8_t sreg = SREG;  // Save interrupt state
    cli();                 // Disable interrupts

    // read_bme();

    // uint16_t len = 16; // PM data + BME data
    uint16_t len = 23; // PM data (4) + SDS011 diagnostics (19)

    msg.buffer[0] = (uint8_t)(len >> 8);
    msg.buffer[1] = (uint8_t)(len & 0xFF);

    for (uint8_t i = 0; i < PM_PACKET_SIZE; i++) {
        msg.buffer[sizeof(uint16_t) + i] = pm_data_finished[i];
    }
    msg.buffer[6] = (uint8_t)(sds_valid_frames & 0xFF);
    msg.buffer[7] = (uint8_t)(sds_valid_frames >> 8);
    msg.buffer[8] = (uint8_t)(sds_checksum_fail_frames & 0xFF);
    msg.buffer[9] = (uint8_t)(sds_checksum_fail_frames >> 8);
    msg.buffer[10] = (uint8_t)(sds_uart_error_bytes & 0xFF);
    msg.buffer[11] = (uint8_t)(sds_uart_error_bytes >> 8);
    msg.buffer[12] = (uint8_t)(sds_rx_bytes & 0xFF);
    msg.buffer[13] = (uint8_t)(sds_rx_bytes >> 8);
    msg.buffer[14] = (uint8_t)(sds_aa_headers & 0xFF);
    msg.buffer[15] = (uint8_t)(sds_aa_headers >> 8);
    msg.buffer[16] = (uint8_t)(sds_c0_headers & 0xFF);
    msg.buffer[17] = (uint8_t)(sds_c0_headers >> 8);
    msg.buffer[18] = (uint8_t)(sds_c5_headers & 0xFF);
    msg.buffer[19] = (uint8_t)(sds_c5_headers >> 8);
    msg.buffer[20] = (uint8_t)(sds_query_sent & 0xFF);
    msg.buffer[21] = (uint8_t)(sds_query_sent >> 8);
    msg.buffer[22] = (uint8_t)(sds_rx_edges & 0xFF);
    msg.buffer[23] = (uint8_t)(sds_rx_edges >> 8);
    msg.buffer[24] = reset_flags_snapshot;
    // for (uint8_t i = 0; i < BME_PACKET_SIZE; i++) {
    //     msg.buffer[sizeof(uint16_t) + PM_PACKET_SIZE + i] = bme_data[i];
    // }

    msg.current_idx = 0;
    msg.len = len + 2;
    needs_fill = false;

    SREG = sreg;  // Restore interrupt state (safe even if read_bme() blocks)
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
    reset_flags_snapshot = RSTCTRL.RSTFR;
    RSTCTRL.RSTFR = reset_flags_snapshot;

    slave_init();
    // calibrate_bme();
    fill_msg();  // pre-fill with zeros so slave can ACK before SDS011 is ready
    // set_sleep_mode(SLEEP_MODE_IDLE);
    sei();       // enable interrupts early so TWI ISR can respond to master
    _delay_ms(1000); // Wait for sds011 to power on

    // SDS011 may ignore the first wakeup after power-on.
    send_sds011_wakeup();
    _delay_ms(200);
    send_sds011_wakeup();
    _delay_ms(500);

    // Query mode + explicit query loop is the most deterministic flow for SDS011.
    set_sds011_query_mode();
    _delay_ms(500);

    // Keep watchdog disabled while debugging serial receive stability.
    while (1) {
        poll_sds011_uart();
        // sleep_mode();
        if (needs_fill) {
            // Query at a slower cadence to reduce sensor command pressure.
            // With a 1s master polling loop and cooldown=2, this is ~1 query every 3s.
            if (sds_query_cooldown == 0) {
                request_sds011_query_with_retry();
                sds_query_cooldown = 2;
            } else {
                sds_query_cooldown--;
                poll_sds011_uart_ms(20);
            }
            fill_msg();
        }
    }
    return 0;
}
