#ifndef VEMAR_I2C_H
#define VEMAR_I2C_H

#include "common.h"

typedef enum {
	I2C_PS1 = 0x00,
	I2C_PS4 = 0x01,
	I2C_PS16 = 0x02,
	I2C_PS64 = 0x03
} i2c_ps_t;

void I2C_init(i2c_ps_t prescaler);

/**
 * @brief Set the slave address (7 bits) of the device
 * @param address 7-bit slave address
 */
inline void I2C_set_address(byte_t address)
{
	TWAR = (address << 1);
}

/**
 * @brief Transmit data to the I2C bus
 * @param address I2C address of the device to transmit to
 * @param data Pointer to the data to transmit
 * @param length Length of the data to transmit
 */
void I2C_read(byte_t address, byte_t *data, length_t length);

/**
 * @brief Receive data from the I2C bus
 * @param address I2C address of the device to receive from
 * @param data Pointer to the data to receive
 * @param length Length of the data to receive
 */
void I2C_write(byte_t address, byte_t *data, length_t length);

#endif // VEMAR_I2C_H

/**
 * @file i2c.h
 * @brief I2C header file
 * @version 0.0.1
 * @author Christian Hugon
 */
