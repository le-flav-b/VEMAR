#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include <util/delay.h>

#include "i2c.h"
#include "bme.h"

#define ATMOSPHERE_ADDRESS 0x09
#define RAW_PM_PACKET_SIZE 10
#define PM_PACKET_SIZE 4
#define BME_PACKET_SIZE 12
#define ATMOSPHERE_MAX_LEN 64

#define wdt_reset() __asm__ __volatile__ ("wdr") 

void BME_init(void);
bool_t BME_do_read(void);

uint16_t BME_pm25(void);
uint16_t BME_pm10(void);
int16_t BME_temperature(void);
uint16_t BME_pressure(void);
uint16_t BME_humidity(void);

#endif