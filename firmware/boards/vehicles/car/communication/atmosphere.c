#include "serial.h"
#include "atmosphere.h"

#define BME280_ADDR          0x76
#define BME280_REG_RESET     0xE0
#define BME280_REG_CTRL_HUM  0xF2
#define BME280_REG_STATUS    0xF3
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG    0xF5
#define BME280_REG_CALIB_00  0x88
#define BME280_REG_CALIB_26  0xE1
#define BME280_REG_DATA      0xF7

static struct bme_calib calib = {0};
static uint8_t bme_data[BME_PACKET_SIZE] = {0};
static uint8_t buffer[ATMOSPHERE_MAX_LEN] = {0};

static int32_t unpack_s32_be(const uint8_t *buf) {
    return ((int32_t)buf[0] << 24) |
           ((int32_t)buf[1] << 16) |
           ((int32_t)buf[2] << 8) |
           (int32_t)buf[3];
}

static uint32_t unpack_u32_be(const uint8_t *buf) {
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8) |
           (uint32_t)buf[3];
}

static uint16_t unpack_u16_le(const uint8_t *buf) {
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

static int8_t bme_write_reg(uint8_t reg, uint8_t value) {
    int8_t err = i2c_start((BME280_ADDR << 1), FALSE);
    if (err != 0) { i2c_stop(); return err; }
    err = i2c_write(reg);
    if (err != 0) { i2c_stop(); return err; }
    err = i2c_write(value);
    i2c_stop();
    return err;
}

static int8_t bme_read_regs(uint8_t reg, uint8_t *data, uint8_t len) {
    int8_t err = i2c_start((BME280_ADDR << 1), FALSE);
    if (err != 0) { i2c_stop(); return err; }
    err = i2c_write(reg);
    i2c_stop();
    if (err != 0) return err;

    err = i2c_start((BME280_ADDR << 1) | 1, FALSE);
    if (err != 0) { i2c_stop(); return err; }

    for (uint8_t i = 0; i < len; i++) {
        int16_t rx = (i == (uint8_t)(len - 1)) ? i2c_read_nack() : i2c_read_ack();
        if (rx < 0) { i2c_stop(); return (int8_t)rx; }
        data[i] = (uint8_t)rx;
    }
    i2c_stop();
    return 0;
}

static bool_t bme_wait_ready(uint16_t timeout_ms) {
    uint8_t status = 0;
    while (timeout_ms-- > 0) {
        if (bme_read_regs(BME280_REG_STATUS, &status, 1) == 0) {
            if ((status & 0x09) == 0) return TRUE;
        }
        _delay_ms(1);
    }
    return FALSE;
}

// static void calibrate_bme(void) {
//     uint8_t calib_tp[26] = {0};
//     uint8_t calib_h[7] = {0};
//     uint8_t calib_data[28] = {0};

//     uint8_t chip_id = 0;
//     int8_t id_err = bme_read_regs(0xD0, &chip_id, 1);
//     if (id_err != 0) {
//         SERIAL_print(str, "BME280 ID read error: ");
//         SERIAL_println(int, id_err);
//         return;
//     }
//     if (chip_id != 0x60) {
//         SERIAL_print(str, "BME280 wrong chip ID: 0x");
//         SERIAL_println(hex, chip_id, 2);
//         return;
//     }

//     (void)bme_write_reg(BME280_REG_RESET, 0xB6);
//     _delay_ms(10);

//     (void)bme_write_reg(BME280_REG_CTRL_HUM, 0x01);
//     (void)bme_write_reg(BME280_REG_CONFIG, 0xC0);
//     (void)bme_write_reg(BME280_REG_CTRL_MEAS, 0x27);

//     int8_t err1 = bme_read_regs(BME280_REG_CALIB_00, calib_tp, sizeof(calib_tp));
//     int8_t err2 = bme_read_regs(BME280_REG_CALIB_26, calib_h, sizeof(calib_h));
//     if (err1 != 0 || err2 != 0) {
//         SERIAL_println(str, "BME280 calib read failed");
//         return;
//     }

//     for (uint8_t i = 0; i < 24; i++) calib_data[i] = calib_tp[i];
//     calib_data[24] = calib_tp[25];
//     calib_data[25] = calib_h[0];
//     calib_data[26] = calib_h[1];
//     calib_data[27] = calib_h[2];

//     load_bme280_calib(calib_data, &calib);
//     calib.dig_H4 = ((int16_t)calib_h[3] << 4) | (calib_h[4] & 0x0F);
//     calib.dig_H5 = ((int16_t)calib_h[5] << 4) | (calib_h[4] >> 4);
//     calib.dig_H6 = (int8_t)calib_h[6];

//     SERIAL_println(str, "BME280 calibrated OK");
// }

// static void bme_do_read(void) {
//     uint8_t raw[8] = {0};
//     if (!bme_wait_ready(50)) return;
//     if (bme_read_regs(BME280_REG_DATA, raw, sizeof(raw)) != 0) return;

//     BME280_S32_t adc_P = (BME280_S32_t)(((uint32_t)raw[0] << 12) | ((uint32_t)raw[1] << 4) | ((uint32_t)raw[2] >> 4));
//     BME280_S32_t adc_T = (BME280_S32_t)(((uint32_t)raw[3] << 12) | ((uint32_t)raw[4] << 4) | ((uint32_t)raw[5] >> 4));
//     BME280_S32_t adc_H = (BME280_S32_t)(((uint32_t)raw[6] << 8)  | (uint32_t)raw[7]);

//     int32_t  temp_comp = BME280_compensate_T_int32(adc_T, &calib);
//     uint32_t pres_comp = BME280_compensate_P_int64(adc_P, &calib);
//     uint32_t hum_comp  = bme280_compensate_H_int32(adc_H, &calib);

//     bme_data[0]  = (uint8_t)((temp_comp >> 24) & 0xFF);
//     bme_data[1]  = (uint8_t)((temp_comp >> 16) & 0xFF);
//     bme_data[2]  = (uint8_t)((temp_comp >> 8)  & 0xFF);
//     bme_data[3]  = (uint8_t)(temp_comp          & 0xFF);
//     bme_data[4]  = (uint8_t)((pres_comp >> 24) & 0xFF);
//     bme_data[5]  = (uint8_t)((pres_comp >> 16) & 0xFF);
//     bme_data[6]  = (uint8_t)((pres_comp >> 8)  & 0xFF);
//     bme_data[7]  = (uint8_t)(pres_comp          & 0xFF);
//     bme_data[8]  = (uint8_t)((hum_comp >> 24)  & 0xFF);
//     bme_data[9]  = (uint8_t)((hum_comp >> 16)  & 0xFF);
//     bme_data[10] = (uint8_t)((hum_comp >> 8)   & 0xFF);
//     bme_data[11] = (uint8_t)(hum_comp           & 0xFF);
// }

// Read PM data from slave and BME280 directly, then print all readings
// void read_and_print_sensors(void) {


//     // Read PM packet from slave - now just [PM:4][sds_valid_frames_lo][sds_rx_bytes_lo]
//     int8_t err = i2c_read_packet(ATMOSPHERE_ADDRESS, buffer);
//     if (err != 0) {
//         SERIAL_print(str, "Slave read error: ");
//         SERIAL_println(int, err);
//         return;
//     }

//     // Read BME280 directly on this bus
//     bme_do_read();

//     uint16_t pm25_raw = unpack_u16_le(&buffer[0]);
//     uint16_t pm10_raw = unpack_u16_le(&buffer[2]);

//     int32_t  temp_centi      = unpack_s32_be(&bme_data[0]);
//     uint32_t pressure_q24_8  = unpack_u32_be(&bme_data[4]);
//     uint32_t humidity_q22_10 = unpack_u32_be(&bme_data[8]);

//     // float pm25_ugm3 = (float)pm25_raw / 10.0f;
//     // float pm10_ugm3 = (float)pm10_raw / 10.0f;
//     // float temp_c      = (float)temp_centi / 100.0f;
//     // float pressure_pa = (float)pressure_q24_8 / 25600.0f;
//     // float humidity_rh = (float)humidity_q22_10 / 1024.0f;

//     SERIAL_println(str, "=== Atmosphere Readings ===");
//     SERIAL_print(str, "PM2.5: ");
//     // SERIAL_print(long, (long)pm25_ugm3);
//     SERIAL_print(uint, pm25_raw);
//     SERIAL_println(str, " ug/m3");
//     SERIAL_print(str, "PM10:  ");
//     // SERIAL_print(long, (long)pm10_ugm3);
//     SERIAL_print(uint, pm10_raw);
//     SERIAL_println(str, " ug/m3");
//     SERIAL_print(str, "Temp:  ");
//     // SERIAL_print(long, (long)temp_c);
//     SERIAL_print(int, temp_centi);
//     SERIAL_println(str, " C");
//     SERIAL_print(str, "Press: ");
//     // SERIAL_print(long, (long)pressure_pa);
//     SERIAL_print(uint, pressure_q24_8);
//     SERIAL_println(str, " Pa");
//     SERIAL_print(str, "Hum:   ");
//     // SERIAL_print(long, (long)humidity_rh);
//     SERIAL_print(uint, humidity_q22_10);
//     SERIAL_println(str, " %RH");
//     uint8_t sds_frames = buffer[PM_PACKET_SIZE];
//     uint8_t sds_rx     = buffer[PM_PACKET_SIZE + 1];
//     SERIAL_print(str, "SDS frames: ");
//     SERIAL_print(uint, sds_frames);
//     SERIAL_print(str, "  RX bytes: ");
//     SERIAL_println(uint, sds_rx);
//     SERIAL_println(str, "");
// }

void BME_init(void) {
    uint8_t calib_tp[26] = {0};
    uint8_t calib_h[7] = {0};
    uint8_t calib_data[28] = {0};

    uint8_t chip_id = 0;
    int8_t id_err = bme_read_regs(0xD0, &chip_id, 1);
    if (id_err != 0) {
        SERIAL_print(str, "BME280 ID read error: ");
        SERIAL_println(int, id_err);
        return;
    }
    if (chip_id != 0x60) {
        SERIAL_print(str, "BME280 wrong chip ID: 0x");
        SERIAL_println(hex, chip_id, 2);
        return;
    }

    (void)bme_write_reg(BME280_REG_RESET, 0xB6);
    _delay_ms(10);

    (void)bme_write_reg(BME280_REG_CTRL_HUM, 0x01);
    (void)bme_write_reg(BME280_REG_CONFIG, 0xC0);
    (void)bme_write_reg(BME280_REG_CTRL_MEAS, 0x27);

    int8_t err1 = bme_read_regs(BME280_REG_CALIB_00, calib_tp, sizeof(calib_tp));
    int8_t err2 = bme_read_regs(BME280_REG_CALIB_26, calib_h, sizeof(calib_h));
    if (err1 != 0 || err2 != 0) {
        SERIAL_println(str, "BME280 calib read failed");
        return;
    }

    for (uint8_t i = 0; i < 24; i++) calib_data[i] = calib_tp[i];
    calib_data[24] = calib_tp[25];
    calib_data[25] = calib_h[0];
    calib_data[26] = calib_h[1];
    calib_data[27] = calib_h[2];

    load_bme280_calib(calib_data, &calib);
    calib.dig_H4 = ((int16_t)calib_h[3] << 4) | (calib_h[4] & 0x0F);
    calib.dig_H5 = ((int16_t)calib_h[5] << 4) | (calib_h[4] >> 4);
    calib.dig_H6 = (int8_t)calib_h[6];

    SERIAL_println(str, "BME280 calibrated OK");
}

bool_t BME_do_read(void) {
    int8_t err = i2c_read_packet(ATMOSPHERE_ADDRESS, buffer);
    if (err != 0) {
        SERIAL_print(str, "Slave read error: ");
        SERIAL_println(int, err);
        return FALSE;
    }

    uint8_t raw[8] = {0};
    if (!bme_wait_ready(50)) return FALSE;
    if (bme_read_regs(BME280_REG_DATA, raw, sizeof(raw)) != 0) return FALSE;

    BME280_S32_t adc_P = (BME280_S32_t)(((uint32_t)raw[0] << 12) | ((uint32_t)raw[1] << 4) | ((uint32_t)raw[2] >> 4));
    BME280_S32_t adc_T = (BME280_S32_t)(((uint32_t)raw[3] << 12) | ((uint32_t)raw[4] << 4) | ((uint32_t)raw[5] >> 4));
    BME280_S32_t adc_H = (BME280_S32_t)(((uint32_t)raw[6] << 8)  | (uint32_t)raw[7]);

    int32_t  temp_comp = BME280_compensate_T_int32(adc_T, &calib);
    uint32_t pres_comp = BME280_compensate_P_int64(adc_P, &calib);
    uint32_t hum_comp  = bme280_compensate_H_int32(adc_H, &calib);

    bme_data[0]  = (uint8_t)((temp_comp >> 24) & 0xFF);
    bme_data[1]  = (uint8_t)((temp_comp >> 16) & 0xFF);
    bme_data[2]  = (uint8_t)((temp_comp >> 8)  & 0xFF);
    bme_data[3]  = (uint8_t)(temp_comp          & 0xFF);
    bme_data[4]  = (uint8_t)((pres_comp >> 24) & 0xFF);
    bme_data[5]  = (uint8_t)((pres_comp >> 16) & 0xFF);
    bme_data[6]  = (uint8_t)((pres_comp >> 8)  & 0xFF);
    bme_data[7]  = (uint8_t)(pres_comp          & 0xFF);
    bme_data[8]  = (uint8_t)((hum_comp >> 24)  & 0xFF);
    bme_data[9]  = (uint8_t)((hum_comp >> 16)  & 0xFF);
    bme_data[10] = (uint8_t)((hum_comp >> 8)   & 0xFF);
    bme_data[11] = (uint8_t)(hum_comp           & 0xFF);

    return TRUE;
}

uint16_t BME_pm25(void) { return unpack_u16_le(&buffer[0]); }

uint16_t BME_pm10(void) { return unpack_u16_le(&buffer[2]); }

int16_t BME_temperature(void) {
    return ((int16_t)(unpack_s32_be(&bme_data[0]) / 10));
}

uint16_t BME_pressure(void) {
    return ((int16_t)(unpack_s32_be(&bme_data[4]) / 2560));
}
uint16_t BME_humidity(void) {
    return ((int16_t)(unpack_u32_be(&bme_data[8]) / 102));
}

// int main(void) {
//     // Serial.begin(9600);
//     SERIAL_init();
//     SERIAL_println(str, "Atmosphere Master Starting...");

//     i2c_init();
//     _delay_ms(100);

//     calibrate_bme();

//     SERIAL_println(str, "Waiting for slave data...");
//     _delay_ms(500);

//     while (1) {
//         read_and_print_sensors();
//         _delay_ms(1000);
//     }

//     return 0;
// }
