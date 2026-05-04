/* #include "i2c.h"
#include "lidar.h"
#include "servo.h"
#include "led.h" */

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define MSK(x) (1 << (x))

int main(void)
{
    // Debug LEDs
    DDRD |= MSK(PD6) | MSK(PD7);

    // Ensure TWI pins are inputs
    DDRD &= ~(MSK(PD0) | MSK(PD1));

    // Set slave address = 0x0C
    TWAR = (0x0C << 1);

    // Enable TWI + ACK (NO interrupt)
    TWCR = MSK(TWEN) | MSK(TWEA);

    while (1)
    {
        // Blink PD6 so we know MCU runs
        PORTD ^= MSK(PD6);
        _delay_ms(200);

        // Check if TWI event happened
        if (TWCR & MSK(TWINT))
        {
            // We got something → light PD7
            PORTD |= MSK(PD7);

            // Clear flag and continue
            TWCR = MSK(TWINT) | MSK(TWEN) | MSK(TWEA);
        }
    }
}


/* #include "utils.h"
#include <avr/interrupt.h>

ISR(TWI_vect)
{
	PORTD |= MSK(PD7);
}

int main()
{
	TWAR = (0x0C << 1);
	TWCR = MSK(TWINT) | MSK(TWEA) | MSK(TWEN) | MSK(TWIE);
	sei();

	DDRD |= MSK(PD6) | MSK(PD7);
	while (1)
	{
		PORTD ^= MSK(PD6);
		_delay_ms(500);
	}
} */

/*
int main()
{
	led_init();
	i2c_slave_init();
	lidar_init();
	//servo_init(); // blocks
	led_green_on();

	while (1)
	{
		led_green_toggle();
		_delay_ms(500);
		/* if (rx_data)
		{
			switch (rx_data)
			{
				case I2C_CMD_SERVO_A_L1: servo_a_l1(); break;
				case I2C_CMD_SERVO_A_L2: servo_a_l2(); break;
				case I2C_CMD_SERVO_A_R1: servo_a_r1(); break;
				case I2C_CMD_SERVO_A_R2: servo_a_r2(); break;
				case I2C_CMD_SERVO_B_L1: servo_b_l1(); break;
				case I2C_CMD_SERVO_B_L2: servo_b_l2(); break;
				case I2C_CMD_SERVO_B_R1: servo_b_r1(); break;
				case I2C_CMD_SERVO_B_R2: servo_b_r2(); break;
				case I2C_CMD_LIDAR_ON: lidar_on(); break;
				case I2C_CMD_LIDAR_OFF: lidar_off(); break;
				case I2C_CMD_LIDAR_RESET: lidar_reset(); break;
				case I2C_CMD_LIDAR_TAKE_MEASUREMENT: lidar_take_mesurement(); break;
				case I2C_CMD_LIDAR_IS_READY: tx_data = lidar_is_ready(); break;
				case I2C_CMD_LIDAR_GET_IMAGE: // TODO
				default: break;
			}
			rx_data = 0;
		} 
	}
}
*/