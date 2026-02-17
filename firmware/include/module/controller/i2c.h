#ifndef VEMAR_I2C_H
#define VEMAR_I2C_H

#include <util/twi.h>
#include "common.h"

#define I2C_TWBR(freq, prescaler) ((F_CPU / freq - 16) / 2 / prescaler)

#define I2C_SDA PORTC4 ///< Serial Interface Data, mapped to PC4
#define I2C_SCL PORTC5 ///< Serial Interface Clock, mapped to PC5

#define I2C_PRESCALER_1 0x00
#define I2C_PRESCALER_4 0x01
#define I2C_PRESCALER_16 0x02
#define I2C_PRESCALER_64 0x03

#define I2C_MASK_PRESCALER 0x03


void I2C_init(uint8_t bitrate, uint8_t prescaler);

/**
 * @brief Set the slave address (7 bits) of the device
 * @param address 7-bit slave address
 * @details
 * The `TWAR` should be loaded with the 7-bit slave address (in the seven most
 * significant bits of `TWAR`) to which the TWI will respond when programmed as
 * a slave transmitter or receiver, and not nedded in the master modes.
 * In multi-master systems, `TWAR` must be set in masters which can be addressed
 * as slaves by other masters.
 * The LSB of `TWAR` is used to enable recognition of the general call address
 * (0x00). There is an associated address comparator that looks for the slave
 * address (or general call address if enabled) in the received serial address.
 * If a match is found, an interrupt request is generated.
 */
inline void I2C_set_address(byte_t address)
{
	TWAR = (address << 1);
}

/**
 * @brief Set the prescaler of the I2C
 * @param prescaler Prescaler value
 */
inline void I2C_set_prescaler(byte_t prescaler)
{	
	BIT_write(TWSR, prescaler, I2C_MASK_PRESCALER);
}

inline void I2C_set_bitrate(byte_t frequency);

/**
 * @brief Check whether the interrupt flag is set
 * @return Non-zero value if the interrupt flag is set, otherwise `0`
 * @details
 * `TWINT` flag is set by hardware when the TWI has finished its current job
 * and expects application software response.
 * If the I-bit in `SREG` and `TWIE` in TWCR are set, the MCU will jump to
 * the TWI interrupt vector.
 * While the `TWINT` flag is set, SCL low period is stretched.
 * The `TWINT` flag must be cleared by software by writing a logic `1` to it.
 * Note that this flag is not automatically cleared by hardware when executing
 * the interrupt routine.
 * Also not that clearing this flag start the operation of the TWI,
 * so all accesses to the TWI address register (`TWAR`), TWI status register
 * (`RWSR`), and TWI data register (`TWDR`) must be complete before clearing
 * this flag.
 */
inline bool_t I2C_is_interrupt_set(void)
{
	return (0 != BIT_read(TWCR, BIT(TWINT)));
}

/**
 * @brief Transmit data to the I2C bus
 * @param address I2C address of the device to transmit to
 * @param data Pointer to the data to transmit
 * @param length Length of the data to transmit
 */
void I2C_transmit(byte_t address, byte_t *data, byte_t length);

/**
 * @brief Receive data from the I2C bus
 * @param address I2C address of the device to receive from
 * @param data Pointer to the data to receive
 * @param length Length of the data to receive
 */
void I2C_receive(byte_t address, byte_t *data, byte_t length);

#endif // VEMAR_I2C_H

/**
 * @file i2c.h
 * @brief I2C header file
 * @version 0.0.1
 * @author Christian Hugon
 */
