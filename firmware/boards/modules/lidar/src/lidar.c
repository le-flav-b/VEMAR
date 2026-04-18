#include "lidar.h"

static void _lidar_config_pwm(void)
{
    // Fast PWM, mode 14 (WGM33:0 = 1110), prescaler = 1
	SET_MSK(TCCR3A, MSK(COM3A1) | MSK(WGM31));
	SET_MSK(TCCR3B, MSK(WGM33)  | MSK(WGM32) | MSK(CS30));

	// Duty cycle and frequency configuration
    ICR3  = (F_CPU / LIDAR_MOTOR_PWM_FREQ) - 1;
    OCR3A = (ICR3 + 1) * LIDAR_MOTOR_PWM_DUTY_CYCLE / 100UL - 1;
}

static void _lidar_config_uart(void) {} // TODO

void lidar_init(void)
{
	GPIO_OUTPUT(LIDAR_EN); lidar_off();
	GPIO_OUTPUT(LIDAR_PWM); _lidar_config_pwm();
	_lidar_config_uart();
}

void lidar_on(void) { GPIO_SET(LIDAR_EN); }
void lidar_off(void) { GPIO_CLEAR(LIDAR_EN); }
