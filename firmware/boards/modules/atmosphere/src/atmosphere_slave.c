#include "atmosphere.h"

static volatile bool needs_fill = true;
volatile struct i2cMessage msg = {0};
volatile uint8_t pm_data_unfinished[RAW_PM_PACKET_SIZE] = {0};
volatile uint8_t pm_data_finished[PM_PACKET_SIZE] = {0};
// volatile uint8_t bme_data[BME_PACKET_SIZE] = {0};
static volatile uint8_t uart_idx = 0;
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

void send_sds011_wakeup(void) {
    // AA B4 06 01 01 00*10 FF FF checksum AB
    // checksum = (0x06 + 0x01 + 0x01 + 0xFF + 0xFF) % 256 = 0x06
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x06, 0xAB
    };
    for (uint8_t i = 0; i < 19; i++) {
        uart_send_byte(cmd[i]);
    }
}

void set_sds011_active_mode(void) {
    // AA B4 02 01 00*11 FF FF checksum AB
    // checksum = (0x02 + 0x01 + 0xFF + 0xFF) % 256 = 0x01
    const uint8_t cmd[19] = {
        0xAA, 0xB4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x01, 0xAB
    };
    for (uint8_t i = 0; i < 19; i++) {
        uart_send_byte(cmd[i]);
    }
}

ISR(USART0_RXC_vect) {
    uint8_t status = USART0.RXDATAH;
    uint8_t byte = USART0.RXDATAL;

    if (status & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm)) {
        uart_idx = 0;
        return;
    }

    // Sync on start byte 0xAA
    if (uart_idx == 0 && byte != 0xAA) {
        return;
    }
    // Second byte must be 0xC0 (PM data response ID); discard command responses
    if (uart_idx == 1 && byte != 0xC0) {
        uart_idx = 0;
        return;
    }

    pm_data_unfinished[uart_idx++] = byte;

    if (uart_idx == RAW_PM_PACKET_SIZE) {
        for (uint8_t i = 2; i < 6; i++) {
            pm_data_finished[i - 2] = pm_data_unfinished[i];
        }
        toggle_led(); // Blink once per complete PM frame
        uart_idx = 0;
    }
}

ISR(TWI0_TWIS_vect) {
    // APIF - Address or Stop Interrupt Flag (master calls start)
    if (TWI0.SSTATUS & TWI_APIF_bm) {
        msg.current_idx = 0;
        if (msg.len == 0) {
            i2c_slave_nack();
        } else {
            i2c_slave_ack();
        }
    }
    // Master reading
    else if (TWI0.SSTATUS & TWI_DIF_bm) {
        if (TWI0.SSTATUS & TWI_DIR_bm) {
            // DIR=1: master is reading (slave transmitting)
            if (msg.current_idx < msg.len) {
                // Send next byte
                i2c_slave_transmit(msg.buffer[msg.current_idx++]);
            } else {
                // No more data, wait for master to NACK or stop
                i2c_slave_ack();
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
    PORTA.PIN2CTRL |= PORT_PULLUPEN_bm; // Keep RX high when line is idle/open
    USART0.CTRLA = USART_RXCIE_bm; // Enable RX complete interrupt
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
    uint16_t len = 4; // PM data only for now

    msg.buffer[0] = (uint8_t)(len >> 8);
    msg.buffer[1] = (uint8_t)(len & 0xFF);

    for (uint8_t i = 0; i < PM_PACKET_SIZE; i++) {
        msg.buffer[sizeof(uint16_t) + i] = pm_data_finished[i];
    }
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
    slave_init();
    // calibrate_bme();
    fill_msg();  // pre-fill with zeros so slave can ACK before SDS011 is ready
    // set_sleep_mode(SLEEP_MODE_IDLE);
    sei();       // enable interrupts early so TWI ISR can respond to master
    _delay_ms(1000); // Wait for sds011 to power on
    send_sds011_wakeup();
    set_sds011_active_mode();
    CCP = CCP_IOREG_gc; // enable config change
    WDT.CTRLA = WDT_PERIOD_8KCLK_gc; // 8s timeout
    while (1) {
        // sleep_mode();
        if (needs_fill) {
            fill_msg();
        }
        wdt_reset();  // Reset the watchdog
    }
    return 0;
}
