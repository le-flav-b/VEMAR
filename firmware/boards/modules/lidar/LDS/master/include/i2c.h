#ifndef I2C_H
# define I2C_H

# include "util.h"
# include "twi.h"


# define I2C_READ TW_READ
# define I2C_WRITE TW_WRITE

# define I2C_ACK 1
# define I2C_NACK 0

# define I2C_GET_STATUS			(TWSR & (MSK(TWS7) | MSK(TWS6) | MSK(TWS5) | MSK(TWS4) | MSK(TWS3)))
# define I2C_IS_DONE			GET_VALUE(TWCR, TWINT)
# define I2C_CLEAR_TWINT_MSK	MSK(TWINT) | MSK(TWEN)


void i2c_init(void);
uint8_t i2c_start(const uint8_t sla, const uint8_t mode, uint8_t *status);
uint8_t i2c_write(const uint8_t data, uint8_t *status);
uint8_t i2c_write_n(const uint8_t *data, const uint8_t n, uint8_t *status);
uint8_t i2c_send_byte_to(const uint8_t sla, const uint8_t data, uint8_t *status);
uint8_t i2c_send_to(const uint8_t sla, const uint8_t *data, const uint8_t n, uint8_t *status);
uint8_t i2c_read(const uint8_t ack, uint8_t *buf, uint8_t *status);
uint8_t i2c_read_n(const uint8_t last_ack, uint8_t *buf, uint8_t n, uint8_t *status);
uint8_t i2c_receive_from(const uint8_t sla, const uint8_t last_ack, uint8_t *buf, uint8_t n, uint8_t *status);
void i2c_stop(void);


extern uint8_t g_i2c_status;

# define STR(code) #code
static inline const char *I2C_STATUS_MSG(uint8_t status_code) {
	switch(status_code) {
		case 0x00: return "bus-error";
		case 0x08: return "start";
		case 0x10: return "rep-start";
		case 0x18: return "mt-sla-ack";
		case 0x20: return "mt-sla-nack";
		case 0x28: return "mt-data-ack";
		case 0x30: return "mt-data-nack";
		case 0x38: return "mt-arb-lost or mr-arb-lost";
		case 0x40: return "mr-sla-ack";
		case 0x48: return "mr-sla-nack";
		case 0x50: return "mr-data-ack";
		case 0x58: return "mr-data-nack";
		case 0xF8: return "no-info";
		default: return "unknown";
	}
}


/* Register Description
TWBR	Bit Rate Register				Set the i2c frequency (depending on prescaler in TWSR)
TWCR	Control Register				Main register, manage configuration for all i2c communications
TWSR	Status Register					Store the status of the last step + Set i2c prescaler
TWDR	Data Register					Data register for write and read, for slave and master (and an address is data)
TWAR	(Slave) Address Register		For the slave, set the address that will be recognize
TWAMR	(Slave) Address Mask Register	For the slave, set a mask of recognition


TWCR :
- TWINT		Interupt Flag					Is i2c busy ?
- TWEA		Enable Acknowledge Bit			Set state code when data received
- TWSTA		START Condition Bit				Send a start to the slave
- TWSTO		STOP Condition Bit				Send a stop to the slave
- TWWC		Write Collision Flag			Was the i2c busy when attempting to write ?
- TWEN		Enable Bit						Enable i2c
- TWIE		Interrupt Enable				Interrupt as long as TWINT is high

TWSR :
- TWS7:3	Status							The status of the last step
- TWPS1:0	Prescaler Bits					Set the prescaler for the i2c frequency

TWAR :
- TWA6:0	(Slave) Address Register		My i2c address
- TWGCE		General Call Enable Bit			Set if want to recognize general call (0x00)

TWAMR :
- TWAM6:0	TWI Address Mask				If a bit is set, it will be ignored during address recognition
*/

/* TWINT Management
When an event occurs, TWINT is set and TWSR is updated.
Events :
- transmitted a START/REPEATED START condition.
- transmitted SLA+R/W.
- transmitted an address byte.
- lost arbitration.
- been addressed by own slave address or general call.
- received a data byte.
- STOP or REPEATED START has been received while still addressed as a slave.
- bus error has occurred due to an illegal START or STOP condition.
*/

#endif
