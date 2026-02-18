#include "i2c.h"

#define I2C_WRITING 0
#define I2C_READING 1

static void I2C_transmit_address(byte_t address, byte_t rw)
{
	TWDR = ((address << 1) | (rw));
	TWCR = (BIT(TWINT) | BIT(TWEN));
	WAIT_UNTIL(I2C_is_interrupt_set());
	return ((TW_MT_SLA_ACK == TW_STATUS) || (TW_MR_SLA_ACK == TW_STATUS));
}

void I2C_init(uint8_t frequency, uint8_t prescaler)
{
	TWCR = 0;
	TWBR = ;
}

void I2C_start(void)
{
	TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
	WAIT_UNTIL(I2C_is_interrupt_set());
	if ((TW_START == TW_STATUS) || (TW_REP_START == TW_STATUS))
		;
}

void I2C_stop(void)
{
	TWCR = (BIT(TWINT) | BIT(TWSTO) | BIT(TWEN));
	WAIT_UNTIL(I2C_is_interrupt_set());
}

static byte_t I2C_read_ack(void)
{
	TWCR = (BIT(TWINT) | BIT(TWEA) | BIT(TWEN));
	WAIT_UNTIL(I2C_is_interrupt_set());
	if (TW_MR_DATA_ACK == TW_STATUS)
	{
		return (TWDR);
	}
	return (0);
}

static byte_t I2C_read_nack(void)
{
	TWCR = (BIT(TWINT) | BIT(TWEN));
	WAIT_UNTIL(I2C_is_interrupt_set());
	return (TW_MR_DATA_NACK == TW_STATUS);
}

static byte_t I2C_write(byte_t data)
{
	TWDR = data;
	TWCR = (BIT(TWINT) | BIT(TWEN));
	WAIT_UNTIL(I2C_is_interrupt_set());
	return (TW_MT_DATA_ACK == TW_STATUS);
}

void I2C_transmit(byte_t address, byte_t *data, byte_t length)
{
	I2C_start();
	I2C_transmit_address(address, I2C_WRITING);
	for (byte_t i = 0; i < length; ++i)
	{
		I2C_write(data[i]);
	}
	I2C_stop();
}

void I2C_receive(byte_t address, byte_t *data, byte_t length)
{
	I2C_start();
	I2C_transmit_address(address, I2C_READING);
	for (byte_t i = 0; i < length; ++i)
	{
		data[i] = I2C_read_ack();
	}
	I2C_read_nack();
	I2C_stop();
}

extern inline void I2C_set_address(byte_t address);
