#include "atmosphere.h"

static volatile bool needs_fill = false;
volatile struct i2cMessage msg = {0};
volatile uint8_t pm_data_unfinished[PM_PACKET_SIZE] = {0};
volatile uint8_t pm_data_finished[PM_PACKET_SIZE] = {0};
volatile uint8_t bme_data[BME_PACKET_SIZE] = {0};
static volatile uint8_t uart_idx = 0;
static struct bme_calib calib = {0};  // BME280 calibration data


ISR(USART0_RXC_vect) {
    uint8_t byte = USART0.RXDATAL;
    if (uart_idx < PM_PACKET_SIZE) {
        pm_data_unfinished[uart_idx++] = byte;
    }
    if (uart_idx == PM_PACKET_SIZE) {
        memcpy((void *)pm_data_finished, (void *)pm_data_unfinished, PM_PACKET_SIZE);
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
    USART0.BAUD = 103; // 9600 baud at 16MHz
    USART0.CTRLC = USART_CHSIZE_8BIT_gc;
    PORTA.DIRSET = PIN1_bm; // TX
    PORTA.DIRCLR = PIN2_bm; // RX
    USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; // Enable TX and RX
}


void calibrate_bme(void) {
    i2c_switch_to_master();
    uint8_t calib_data[26] = {0};
    int8_t err = -1;
    for (uint8_t retry = 0; retry < 3; retry++) {
        _delay_ms(10);
        err = i2c_read_packet(0x76, calib_data);
        if (err == 0) break;  // Success, exit retry loop
    }
    if (err != 0) {
        // TODO: handle calibration failure (log error, halt, or use default)
    }
    load_bme280_calib(calib_data, &calib);
    i2c_switch_to_slave();
}


void read_bme(void) {
    i2c_switch_to_master();
    uint8_t raw_bme_data[8] = {0};  // Temperature, Pressure, Humidity ADC data
    int8_t err = -1;
    for (uint8_t retry = 0; retry < 3; retry++) {
        _delay_ms(10);
        err = i2c_read_packet(0x76, raw_bme_data);
        if (err == 0) break;  // Success, exit retry loop
    }
    if (err != 0) {
        // TODO: handle persistent BME read failure
    }
    
    // Extract 20-bit ADC values from raw data
    BME280_S32_t adc_T = (raw_bme_data[0] << 12) | (raw_bme_data[1] << 4) | (raw_bme_data[2] >> 4);
    BME280_S32_t adc_P = (raw_bme_data[3] << 12) | (raw_bme_data[4] << 4) | (raw_bme_data[5] >> 4);
    BME280_S32_t adc_H = (raw_bme_data[6] << 8) | raw_bme_data[7];
    
    // Compensate and pack into message buffer as compensated values
    int32_t temp_comp = BME280_compensate_T_int32(adc_T, &calib);
    uint32_t pres_comp = BME280_compensate_P_int64(adc_P, &calib);
    uint32_t hum_comp = bme280_compensate_H_int32(adc_H, &calib);
    
    // Pack compensated values into bme_data (4 bytes each)
    bme_data[0] = (temp_comp >> 24) & 0xFF;
    bme_data[1] = (temp_comp >> 16) & 0xFF;
    bme_data[2] = (temp_comp >> 8) & 0xFF;
    bme_data[3] = temp_comp & 0xFF;
    
    bme_data[4] = (pres_comp >> 24) & 0xFF;
    bme_data[5] = (pres_comp >> 16) & 0xFF;
    bme_data[6] = (pres_comp >> 8) & 0xFF;
    bme_data[7] = pres_comp & 0xFF;
    
    bme_data[8] = (hum_comp >> 24) & 0xFF;
    bme_data[9] = (hum_comp >> 16) & 0xFF;
    bme_data[10] = (hum_comp >> 8) & 0xFF;
    bme_data[11] = hum_comp & 0xFF;
    
    i2c_switch_to_slave();
}

static void fill_msg(void) {
    uint8_t sreg = SREG;  // Save interrupt state
    cli();                 // Disable interrupts

    read_bme();

    uint16_t len = 16; // PM data + BME data

    msg.buffer[0] = (uint8_t)(len >> 8);
    msg.buffer[1] = (uint8_t)(len & 0xFF);

    for (uint8_t i = 0; i < PM_PACKET_SIZE; i++) {
        msg.buffer[sizeof(uint16_t) + i] = pm_data_finished[i];
    }
    for (uint8_t i = 0; i < BME_PACKET_SIZE; i++) {
        msg.buffer[sizeof(uint16_t) + PM_PACKET_SIZE + i] = bme_data[i];
    }

    msg.current_idx = 0;
    msg.len = len + 2;
    needs_fill = false;

    SREG = sreg;  // Restore interrupt state (safe even if read_bme() blocks)
}

void slave_init(void) {
    i2c_init_slave(SLAVE_ADDR);
    CCP = CCP_IOREG_gc; // enable config change
    WDT.CTRLA = WDT_PERIOD_8KCLK_gc; // 8s timeout
    uart_init();
}

int main(void) {
    slave_init();
    calibrate_bme();
    fill_msg();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();
    while (1) {
        sleep_mode();
        if (needs_fill) {
            fill_msg();
        }
        wdt_reset();  // Reset the watchdog
    }
    return 0;
}
