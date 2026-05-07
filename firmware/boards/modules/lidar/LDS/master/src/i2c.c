#include "util.h"
#include "i2c.h"


/* global i2c_status */
uint8_t g_i2c_status = 0;

/*
Init TWI with a frequency of 100kHz
*/
void i2c_init(void)
{
	SET(TWSR, TWPS0); // Set prescaler 4
	TWBR = 18; // Set corresponding value for 100khz
}

/*
Send start signal and SLA+R/W via TWI and wait for each to be transmitted
Return 0 on success
Return 1 and set status if start signal transmission failed, SLA+R/W isn't send
Return 2 and set status if SLA+R/W transmission failed
*/
uint8_t i2c_start(const uint8_t sla, const uint8_t mode, uint8_t *status)
{
	TWCR = I2C_CLEAR_TWINT_MSK | MSK(TWSTA); // Send start signal
	while (!I2C_IS_DONE); // Wait for start signal to be transmitted
	if ((*status = I2C_GET_STATUS) != TW_START && *status != TW_REP_START) return 1; // Failed to transmit start signal
	TWDR = (sla << TWD1) | mode; // Set data register with SLA+R/W
	TWCR = I2C_CLEAR_TWINT_MSK; // Send data
	while (!I2C_IS_DONE); // Wait for SLA+R/W to be transmitted
	if ((*status = I2C_GET_STATUS) != (mode == I2C_WRITE ? TW_MT_SLA_ACK : TW_MR_SLA_ACK)) return 2; // Failed to transmit SLA+R/W
	return 0;
}

/*
Send a byte via TWI and wait for it to be transmitted
Return 0 on success
Return 1 and set status if data transmission failed
*/
uint8_t i2c_write(const uint8_t data, uint8_t *status)
{
	TWDR = data; // Set data register
	TWCR = I2C_CLEAR_TWINT_MSK; // Send data
	while (!I2C_IS_DONE); // Wait for data to be transmitted
	if ((*status = I2C_GET_STATUS) != TW_MT_DATA_ACK) return 1; // Failed to transmit data
	return 0;
}

/*
Send n bytes via TWI and wait for each to be transmitted
Return 0 on success
Return (index + 1) of the byte it where at and set status if a transmission failed
If n > sizeof(data) it can segfault
*/
uint8_t i2c_write_n(const uint8_t *data, const uint8_t n, uint8_t *status)
{
	for (uint8_t i = 0; i < n; i++)
		if (i2c_write(data[i], status))
			return i + 1;
	return 0;
}

/*
Send start signal and n data bytes to sla via TWI and wait for each to be transmitted
Return 0 on success
Return 1 and set status if start signal transmission failed
Return 2 and set status if SLA+R/W transmission failed
Return 3 and set status if data transmission failed
If n > sizeof(data) it can segfault
*/
uint8_t i2c_send_byte_to(const uint8_t sla, const uint8_t data, uint8_t *status)
{
	uint8_t ret;
	if ((ret = i2c_start(sla, I2C_WRITE, status))) return ret;
	if (i2c_write(data, status)) return 3;
	return 0;
}

/*
Send start signal and n data bytes to sla via TWI and wait for each to be transmitted
Return 0 on success
Return 1 and set status if start signal transmission failed
Return 2 and set status if SLA+R/W transmission failed
Return (index + 3) of the data byte it where at and set status if a data transmission failed
If n > sizeof(data) it can segfault
*/
uint8_t i2c_send_to(const uint8_t sla, const uint8_t *data, const uint8_t n, uint8_t *status)
{
	uint8_t ret;
	if ((ret = i2c_start(sla, I2C_WRITE, status))) return ret;
	if ((ret = i2c_write_n(data, n, status))) return ret + 2;
	return 0;
}

/*
Read a byte from TWI and store it in buf
Return 0 on success
Return 1 and set status if data reception failed
*/
uint8_t i2c_read(const uint8_t ack, uint8_t *buf, uint8_t *status)
{
	TWCR = I2C_CLEAR_TWINT_MSK | (MSK(TWEA) * ack); // Indicate that data is ready to be received
	while (!I2C_IS_DONE); // Wait for data to be received
	*buf = TWDR; // Store data
	if ((*status = I2C_GET_STATUS) != (ack ? TW_MR_DATA_ACK : TW_MR_DATA_NACK)) return 1; // Failed to transmit data
	return 0;
}

/*
Read n bytes from TWI and store them in buf
Return 0 on success
Return (index + 1) of the byte it where at and set status if a data reception failed
If n > sizeof(buf) it can segfault
*/
uint8_t i2c_read_n(const uint8_t last_ack, uint8_t *buf, uint8_t n, uint8_t *status)
{
	for (uint8_t i = 0; i < n; i++)
		if (i2c_read((i < n - 1 ? I2C_ACK : last_ack), buf + i, status))
			return i + 1;
	return 0;
}

/*
Send start signal and n data bytes to sla via TWI and wait for each to be transmitted
Return 0 on success
Return 1 and set status if start signal transmission failed
Return 2 and set status if SLA+R/W transmission failed
Return (index + 3) of the data byte it where at and set status if a data reception failed
If n > sizeof(buf) it can segfault
*/
uint8_t i2c_receive_from(const uint8_t sla, const uint8_t last_ack, uint8_t *buf, uint8_t n, uint8_t *status)
{
	uint8_t ret;
	if ((ret = i2c_start(sla, I2C_READ, status))) return ret;
	if ((ret = i2c_read_n(last_ack, buf, n, status))) return ret + 2;
	return 0;
}

/*
Send stop signal via TWI and wait for it to be transmitted
*/
void i2c_stop(void)
{
	TWCR = I2C_CLEAR_TWINT_MSK | MSK(TWSTO); // Send stop signal
	while (GET_VALUE(TWCR, TWSTO)); // Wait for stop signal to be transmitted
}
