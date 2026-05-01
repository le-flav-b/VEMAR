#include "module.h"

#define RAW_PM_PACKET_SIZE 10
#define PM_PACKET_SIZE 4
#define BME_PACKET_SIZE 12
#define ATMOSPHERE_MAX_LEN 64

#define BME280_S32_t int32_t
#define BME280_U32_t uint32_t
#define BME280_S64_t int64_t
#define BME280_U64_t uint64_t

#define BME280_ADDR 0x76
#define BME280_REG_RESET 0xE0
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_STATUS 0xF3
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG 0xF5
#define BME280_REG_CALIB_00 0x88
#define BME280_REG_CALIB_26 0xE1
#define BME280_REG_DATA 0xF7

struct bme_calib
{
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4, dig_H5;
    int8_t dig_H6;
    int32_t t_fine;
};

static struct bme_calib calib = {0};
static uint8_t bme_data[BME_PACKET_SIZE] = {0};

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T, struct bme_calib *calib)
{
    BME280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BME280_S32_t)calib->dig_T1 << 1))) * ((BME280_S32_t)calib->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t)calib->dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)calib->dig_T1))) >> 12) * ((BME280_S32_t)calib->dig_T3)) >> 14;
    calib->t_fine = var1 + var2;
    T = (calib->t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P, struct bme_calib *calib)
{
    BME280_S64_t var1, var2, p;
    var1 = ((BME280_S64_t)calib->t_fine) - 128000;
    var2 = var1 * var1 * (BME280_S64_t)calib->dig_P6;
    var2 = var2 + ((var1 * (BME280_S64_t)calib->dig_P5) << 17);
    var2 = var2 + (((BME280_S64_t)calib->dig_P4) << 35);
    var1 = ((var1 * var1 * (BME280_S64_t)calib->dig_P3) >> 8) + ((var1 * (BME280_S64_t)calib->dig_P2) << 12);
    var1 = (((((BME280_S64_t)1) << 47) + var1)) * ((BME280_S64_t)calib->dig_P1) >> 33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((BME280_S64_t)calib->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BME280_S64_t)calib->dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)calib->dig_P7) << 4);
    return (BME280_U32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H, struct bme_calib *calib)
{
    BME280_S32_t v_x1_u32r;
    v_x1_u32r = (calib->t_fine - ((BME280_S32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)calib->dig_H4) << 20) - (((BME280_S32_t)calib->dig_H5) * v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)calib->dig_H6)) >> 10) * (((v_x1_u32r * ((BME280_S32_t)calib->dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)calib->dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)calib->dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (BME280_U32_t)(v_x1_u32r >> 12);
}

void load_bme280_calib(uint8_t *calib_data, struct bme_calib *calib)
{
    calib->dig_T1 = (uint16_t)calib_data[1] << 8 | calib_data[0];
    calib->dig_T2 = (int16_t)calib_data[3] << 8 | calib_data[2];
    calib->dig_T3 = (int16_t)calib_data[5] << 8 | calib_data[4];
    calib->dig_P1 = (uint16_t)calib_data[7] << 8 | calib_data[6];
    calib->dig_P2 = (int16_t)calib_data[9] << 8 | calib_data[8];
    calib->dig_P3 = (int16_t)calib_data[11] << 8 | calib_data[10];
    calib->dig_P4 = (int16_t)calib_data[13] << 8 | calib_data[12];
    calib->dig_P5 = (int16_t)calib_data[15] << 8 | calib_data[14];
    calib->dig_P6 = (int16_t)calib_data[17] << 8 | calib_data[16];
    calib->dig_P7 = (int16_t)calib_data[19] << 8 | calib_data[18];
    calib->dig_P8 = (int16_t)calib_data[21] << 8 | calib_data[20];
    calib->dig_P9 = (int16_t)calib_data[23] << 8 | calib_data[22];
    calib->dig_H1 = calib_data[24];
    calib->dig_H2 = (int16_t)calib_data[26] << 8 | calib_data[25];
    calib->dig_H3 = calib_data[27];
}

static int32_t unpack_s32_be(const uint8_t *buf)
{
    return ((int32_t)buf[0] << 24) |
           ((int32_t)buf[1] << 16) |
           ((int32_t)buf[2] << 8) |
           (int32_t)buf[3];
}

static uint32_t unpack_u32_be(const uint8_t *buf)
{
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8) |
           (uint32_t)buf[3];
}

static uint16_t unpack_u16_le(const uint8_t *buf)
{
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

static int8_t bme_write_reg(uint8_t reg, uint8_t value)
{
    int8_t err = i2c_start((BME280_ADDR << 1), FALSE);
    if (err != 0)
    {
        i2c_stop();
        return err;
    }
    err = i2c_write(reg);
    if (err != 0)
    {
        i2c_stop();
        return err;
    }
    err = i2c_write(value);
    i2c_stop();
    return err;
}

static int8_t bme_read_regs(uint8_t reg, uint8_t *data, uint8_t len)
{
    int8_t err = i2c_start((BME280_ADDR << 1), FALSE);
    if (err != 0)
    {
        i2c_stop();
        return err;
    }
    err = i2c_write(reg);
    i2c_stop();
    if (err != 0)
        return err;

    err = i2c_start((BME280_ADDR << 1) | 1, FALSE);
    if (err != 0)
    {
        i2c_stop();
        return err;
    }

    for (uint8_t i = 0; i < len; i++)
    {
        int16_t rx = (i == (uint8_t)(len - 1)) ? i2c_read_nack() : i2c_read_ack();
        if (rx < 0)
        {
            i2c_stop();
            return (int8_t)rx;
        }
        data[i] = (uint8_t)rx;
    }
    i2c_stop();
    return 0;
}

static bool_t bme_wait_ready(uint16_t timeout_ms)
{
    uint8_t status = 0;
    while (timeout_ms-- > 0)
    {
        if (bme_read_regs(BME280_REG_STATUS, &status, 1) == 0)
        {
            if ((status & 0x09) == 0)
                return TRUE;
        }
        _delay_ms(1);
    }
    return FALSE;
}

bool_t ATMOSPHERE_init(void)
{
    uint8_t calib_tp[26] = {0};
    uint8_t calib_h[7] = {0};
    uint8_t calib_data[28] = {0};

    uint8_t chip_id = 0;
    int8_t id_err = bme_read_regs(0xD0, &chip_id, 1);
    if (id_err != 0)
    {
        return (FALSE);
    }
    if (chip_id != 0x60)
    {
        return (FALSE);
    }

    (void)bme_write_reg(BME280_REG_RESET, 0xB6);
    _delay_ms(10);

    (void)bme_write_reg(BME280_REG_CTRL_HUM, 0x01);
    (void)bme_write_reg(BME280_REG_CONFIG, 0xC0);
    (void)bme_write_reg(BME280_REG_CTRL_MEAS, 0x27);

    int8_t err1 = bme_read_regs(BME280_REG_CALIB_00, calib_tp, sizeof(calib_tp));
    int8_t err2 = bme_read_regs(BME280_REG_CALIB_26, calib_h, sizeof(calib_h));
    if (err1 != 0 || err2 != 0)
    {
        return (FALSE);
    }

    for (uint8_t i = 0; i < 24; i++)
        calib_data[i] = calib_tp[i];
    calib_data[24] = calib_tp[25];
    calib_data[25] = calib_h[0];
    calib_data[26] = calib_h[1];
    calib_data[27] = calib_h[2];

    load_bme280_calib(calib_data, &calib);
    calib.dig_H4 = ((int16_t)calib_h[3] << 4) | (calib_h[4] & 0x0F);
    calib.dig_H5 = ((int16_t)calib_h[5] << 4) | (calib_h[4] >> 4);
    calib.dig_H6 = (int8_t)calib_h[6];

    return (TRUE);
}

bool_t ATMOSPHERE_fill_packet(uint8_t addr, packet_t *packet)
{
    static uint8_t buffer[ATMOSPHERE_MAX_LEN] = {0};

    if (0 != i2c_read_packet(addr, buffer))
    {
        return (FALSE);
    }

    uint8_t raw[8] = {0};
    if (!bme_wait_ready(50))
    {
        return (FALSE);
    }
    if (bme_read_regs(BME280_REG_DATA, raw, sizeof(raw)) != 0)
    {
        return (FALSE);
    }

    BME280_S32_t adc_P = (BME280_S32_t)(((uint32_t)raw[0] << 12) | ((uint32_t)raw[1] << 4) | ((uint32_t)raw[2] >> 4));
    BME280_S32_t adc_T = (BME280_S32_t)(((uint32_t)raw[3] << 12) | ((uint32_t)raw[4] << 4) | ((uint32_t)raw[5] >> 4));
    BME280_S32_t adc_H = (BME280_S32_t)(((uint32_t)raw[6] << 8) | (uint32_t)raw[7]);

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

    packet->atmosphere.id = PACKET_ID_ATM;
    packet->atmosphere.pm25 = unpack_u16_le(&buffer[0]);
    packet->atmosphere.pm10 = unpack_u16_le(&buffer[2]);
    packet->atmosphere.temperature = unpack_s32_be(&bme_data[0]) / 10;
    packet->atmosphere.pressure = unpack_s32_be(&bme_data[4]) / 2560;
    packet->atmosphere.humidity = unpack_u32_be(&bme_data[8]) / 102;

    VEMAR_DEBUG(str, "=== Atmosphere Readings ===\r\nPM2.5: ");
    VEMAR_DEBUG(uint, packet->atmosphere.pm25);
    VEMAR_DEBUG(str, " ug/m3\r\nPM10: ");
    VEMAR_DEBUG(uint, packet->atmosphere.pm10);
    VEMAR_DEBUG(str, " ug/m3\r\nTemperature: ");
    VEMAR_DEBUG(int, packet->atmosphere.temperature);
    VEMAR_DEBUG(str, " C\r\nPressure: ");
    VEMAR_DEBUG(uint, packet->atmosphere.pressure);
    VEMAR_DEBUG(str, " Pa\r\nHumidity: ");
    VEMAR_DEBUG(uint, packet->atmosphere.humidity);
    VEMAR_DEBUG(str, " %RH\r\n");

    return (TRUE);
}
