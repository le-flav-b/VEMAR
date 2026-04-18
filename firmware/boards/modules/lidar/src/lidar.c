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
	SET_MSK(LIDAR_DDR, MSK(LIDAR_EN_PIN) | MSK(LIDAR_PWM_PIN));
	CLEAR(LIDAR_PORT, LIDAR_EN_PIN);
	_lidar_config_pwm();
	_lidar_config_uart();
}

void lidar_on(void) { SET(LIDAR_PORT, LIDAR_EN_PIN); }
void lidar_off(void) { CLEAR(LIDAR_PORT, LIDAR_EN_PIN); }
