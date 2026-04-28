#ifndef VEMAR_GAS_MODULE_H
#define VEMAR_GAS_MODULE_H

#include <stdint.h>

#define GAS_MODULE_ADDRESS 0x28 /**< I2C address of the Gas module */

// I2C Commands
#define GAS_MODULE_CMD_LED_ON 0x01
#define GAS_MODULE_CMD_LED_OFF 0x02
#define GAS_MODULE_CMD_INCREASE 0x03
#define GAS_MODULE_CMD_DECREASE 0x04
#define GAS_MODULE_CMD_RESET 0xFF

#define GAS_MODULE_DATA_SIZE 2

typedef union
{
    uint8_t buffer[GAS_MODULE_DATA_SIZE];
    struct
    {
        uint8_t a;
        uint8_t b;
        // uint32_t co2;
        // uint32_t o2;
    } data;
} gas_data_t;

#endif // VEMAR_GAS_MODULE_H
