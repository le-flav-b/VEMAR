#include <stdint.h>

#define BME280_S32_t int32_t
#define BME280_U32_t uint32_t
#define BME280_S64_t int64_t
#define BME280_U64_t uint64_t

struct bme_calib {
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

extern BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T, struct bme_calib *calib);
extern BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P, struct bme_calib *calib);
extern BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H, struct bme_calib *calib);
extern void load_bme280_calib(uint8_t *calib_data, struct bme_calib *calib);