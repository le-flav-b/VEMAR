#include <util/twi.h>

#include "i2c.h"

#define I2C_WRITING 0
#define I2C_READING 1

#define I2C_ACK TW_MR_DATA_ACK
#define I2C_NACK TW_MR_DATA_NACK

#define I2C_FREQUENCY 100000UL /**< I2C frequency 100kHz */

#define I2C_TWBR(prescaler) ((F_CPU / I2C_FREQUENCY - 16UL) / 2UL / prescaler)

static inline void I2C_wait(void);
static void I2C_start(void);
static void I2C_stop(void);
static byte_t I2C_receive(byte_t ack);
static void I2C_transmit(byte_t data);
static void I2C_transmit_address(byte_t address, byte_t rw);

void I2C_init(i2c_ps_t prescaler)
{
	TWCR = 0;				   // clear control flags
	TWSR = (0xF8 | prescaler); // set prescaler
	TWBR = (byte_t)(I2C_TWBR(prescaler));
}

void I2C_read(byte_t address, byte_t *data, length_t length)
{
	I2C_start();
	I2C_transmit_address(address, I2C_READING);
	for (byte_t i = 0; i < length; ++i)
	{
		data[i] = I2C_receive(I2C_ACK);
	}
	I2C_receive(I2C_NACK);
	I2C_stop();
}

void I2C_write(byte_t address, byte_t *data, length_t length)
{
	I2C_start();
	I2C_transmit_address(address, I2C_WRITING);
	for (byte_t i = 0; i < length; ++i)
	{
		I2C_transmit(data[i]);
	}
	I2C_stop();
}

void I2C_wait(void)
{
	WAIT_UNTIL(BIT_is_set(TWCR, BIT(TWINT)));
}

void I2C_start(void)
{
	TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
	I2C_wait();

	if ((TW_START != TW_STATUS) && (TW_REP_START != TW_STATUS))
	{
		// error handling
	}
}

void I2C_stop(void)
{
	TWCR = (BIT(TWINT) | BIT(TWSTO) | BIT(TWEN));
}

byte_t I2C_receive(byte_t ack)
{
	TWCR = (BIT(TWINT) | BIT(TWEA) | BIT(TWEN));
	I2C_wait();
	if (ack != TW_STATUS)
	{
		// error handling
	return (0);
}
	return (TWDR);
}

void I2C_transmit(byte_t data)
{
	TWDR = data;
	TWCR = (BIT(TWINT) | BIT(TWEN));
	I2C_wait();
	if (TW_MT_DATA_ACK != TW_STATUS)
	{
		// error handling
	}
}

void I2C_transmit_address(byte_t address, byte_t rw)
{
	TWDR = ((address << 1) | (rw));
	TWCR = (BIT(TWINT) | BIT(TWEN));
	I2C_wait();
	if ((TW_MT_SLA_ACK != TW_STATUS) && (TW_MR_SLA_ACK != TW_STATUS))
{
		// error handling
	} // If ACK not received
}

extern inline void I2C_set_address(byte_t address);
