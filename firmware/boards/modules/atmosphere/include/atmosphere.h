#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include "i2c.h"
#include <util/delay.h>
#include "bme.h"

#define SLAVE_ADDR 0x09
#define RAW_PM_PACKET_SIZE 10
#define PM_PACKET_SIZE 4
#define BME_PACKET_SIZE 12
#define MAX_LEN 64

#define wdt_reset() __asm__ __volatile__ ("wdr") 

static void fill_msg(void);
void uart_init(void);
void read_bme(void);
static void fill_msg(void);
void slave_init(void);

#endif