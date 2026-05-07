#include "i2c.h"
#include "uart.h"

#define SLAVE_ADDR 0x0C

// Commands
#define I2C_CMD_SERVO_A_L1 0x10 // q
#define I2C_CMD_SERVO_A_L2 0x11 // w
#define I2C_CMD_SERVO_A_R1 0x12 // e
#define I2C_CMD_SERVO_A_R2 0x13 // r

#define I2C_CMD_SERVO_B_L1 0x14 // t
#define I2C_CMD_SERVO_B_L2 0x15 // y
#define I2C_CMD_SERVO_B_R1 0x16 // u
#define I2C_CMD_SERVO_B_R2 0x17 // i

#define I2C_CMD_LIDAR_ON  0x20 // o
#define I2C_CMD_LIDAR_OFF 0x21 // p

// read is 'a'

void	put_nbr(uint16_t n)
{
	char	buf[6];
	uint8_t	i = 0;

	uint16_t integer = n / 100;
	uint16_t decimal = n % 100;

	if (integer == 0)
		buf[i++] = '0';
	else
	{
		char tmp[5];
		int j = 0;
		while (integer > 0)
		{
			tmp[j++] = '0' + (integer % 10);
			integer /= 10;
		}
		while (j > 0)
			buf[i++] = tmp[--j];
	}

	buf[i++] = '.';
	buf[i++] = '0' + (decimal / 10);
	buf[i++] = '0' + (decimal % 10);

	for (int j = 0; j < i; j++)
		uart_tx(buf[j]);
}

void readLidar() {
  uint8_t buf[8];
  i2c_start(SLAVE_ADDR, I2C_READ, &g_i2c_status);
  i2c_read_n(I2C_NACK, buf, 8, &g_i2c_status);
  i2c_stop();

  uint16_t d0   = (buf[0] << 8) | buf[1];
  uint16_t d90  = (buf[2] << 8) | buf[3];
  uint16_t d180 = (buf[4] << 8) | buf[5];
  uint16_t d270 = (buf[6] << 8) | buf[7];

  uart_printstr("0°   = ");
  put_nbr(d0);
  uart_printstr("m\n");

  uart_printstr("90°  = ");
  put_nbr(d90);
  uart_printstr("m\n");

  uart_printstr("180° = ");
  put_nbr(d180);
  uart_printstr("m\n");

  uart_printstr("270° = ");
  put_nbr(d270);
  uart_printstr("m\n");

  uart_tx('\n');
}

int main() {
  i2c_init();
  uart_init();
  while (1)
  {
	switch (uart_rx()) {
		case 'q': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_A_L1, &g_i2c_status); i2c_stop(); break;
		case 'w': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_A_L2, &g_i2c_status); i2c_stop(); break;
		case 'e': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_A_R1, &g_i2c_status); i2c_stop(); break;
		case 'r': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_A_R2, &g_i2c_status); i2c_stop(); break;
		case 't': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_B_L1, &g_i2c_status); i2c_stop(); break;
		case 'y': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_B_L2, &g_i2c_status); i2c_stop(); break;
		case 'u': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_B_R1, &g_i2c_status); i2c_stop(); break;
		case 'i': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_SERVO_B_R2, &g_i2c_status); i2c_stop(); break;
		case 'o': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_LIDAR_ON, &g_i2c_status); i2c_stop(); break;
		case 'p': i2c_send_byte_to(SLAVE_ADDR, I2C_CMD_LIDAR_OFF, &g_i2c_status); i2c_stop(); break;
		case 'a': readLidar(); break;
		default: break;
	}
  }
}
