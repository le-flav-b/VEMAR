#ifndef I2C_H
# define I2C_H

# include <avr/interrupt.h>
# include "utils.h"

# define I2C_ADDR 0x0C
# define I2C_DONE TWCR = MSK(TWINT) | MSK(TWEA) | MSK(TWEN) | MSK(TWIE)

/* cmd write one arg */

# define I2C_CMD_SERVO_A_L1 0x10
# define I2C_CMD_SERVO_A_L2 0x11
# define I2C_CMD_SERVO_A_R1 0x12
# define I2C_CMD_SERVO_A_R2 0x13

# define I2C_CMD_SERVO_B_L1 0x14
# define I2C_CMD_SERVO_B_L2 0x15
# define I2C_CMD_SERVO_B_R1 0x16
# define I2C_CMD_SERVO_B_R2 0x17

# define I2C_CMD_LIDAR_ON 0x20
# define I2C_CMD_LIDAR_OFF 0x21
# define I2C_CMD_LIDAR_RESET 0x22

# define I2C_CMD_LIDAR_TAKE_MEASUREMENT 0x30

/* cmd read */

# define I2C_CMD_LIDAR_IS_READY 0x70
# define I2C_CMD_LIDAR_GET_IMAGE 0x71

volatile uint8_t rx_data;
volatile uint8_t tx_data;

inline void i2c_slave_init(void)
{
	TWAR = (I2C_ADDR << 1);
	I2C_DONE; //TWCR = MSK(TWEA) | MSK(TWEN) | MSK(TWIE);
	sei();
}

ISR(TWI_vect)
{
	GPIO_SET(_PD7);
	switch (TWSR & 0xF8)
	{
		/* --- Master writes to us --- */

		case 0x60:  // SLA+W received
			I2C_DONE; break;

		case 0x80:  // data received
			rx_data = TWDR; I2C_DONE; break;

		case 0xA0:  // STOP
			I2C_DONE; break;


		/* --- Master reads from us --- */

		case 0xA8:  // SLA+R received
			TWDR = tx_data; I2C_DONE; break;

		case 0xB8:  // data sent, ACK received
			TWDR = tx_data; I2C_DONE; break;

		case 0xC0:  // data sent, NACK → done
			I2C_DONE; break;

		default:
			I2C_DONE; break;
	}
}

#endif
