#include "motor.h"

static void _motor_left_config_pwm(void)
{
	//SET_MSK(TCCR0A, MSK(WGM01) | MSK(WGM00) | MSK(COM0A1) | MSK(COM0B1));  // Fast PWM, non-inverting
	SET_MSK(TCCR0A, MSK(WGM01) | MSK(WGM00));  // Fast PWM, non-inverting, pin disconnected
	SET_MSK(TCCR0B, MSK(CS01));  // prescaler = 8
}

static void _motor_right_config_pwm(void)
{
	//SET_MSK(TCCR1A, MSK(WGM10) | MSK(COM1A1) | MSK(COM1B1));  // Mode 5 (8-bit fast PWM), non-inverting
	SET_MSK(TCCR1A, MSK(WGM10));  // Mode 5 (8-bit fast PWM), non-inverting, pin disconnected
	SET_MSK(TCCR1B, MSK(WGM12) | MSK(CS11));  // prescaler = 8
}

static void _motor_left_disconnect_pwm_cw(void) { CLEAR_MSK(TCCR0A, MSK(COM0B1)); }
static void _motor_left_disconnect_pwm_ccw(void) { CLEAR_MSK(TCCR0A, MSK(COM0A1)); }
static void _motor_right_disconnect_pwm_cw(void) { CLEAR_MSK(TCCR1A, MSK(COM1A1)); }
static void _motor_right_disconnect_pwm_ccw(void) { CLEAR_MSK(TCCR1A, MSK(COM1B1)); }
static void _motor_left_connect_pwm_cw(void) { SET_MSK(TCCR0A, MSK(COM0B1)); }
static void _motor_left_connect_pwm_ccw(void) { SET_MSK(TCCR0A, MSK(COM0A1)); }
static void _motor_right_connect_pwm_cw(void) { SET_MSK(TCCR1A, MSK(COM1A1)); }
static void _motor_right_connect_pwm_ccw(void) { SET_MSK(TCCR1A, MSK(COM1B1)); }

static void _motor_left_disable_cw(void) { _motor_left_disconnect_pwm_cw(); GPIO_CLEAR(MOTOR_LEFT_PWM_CW); GPIO_CLEAR(MOTOR_LEFT_EN_CW); }
static void _motor_left_disable_ccw(void) { _motor_left_disconnect_pwm_ccw(); GPIO_CLEAR(MOTOR_LEFT_PWM_CCW); GPIO_CLEAR(MOTOR_LEFT_EN_CCW); }
static void _motor_right_disable_cw(void) { _motor_right_disconnect_pwm_cw(); GPIO_CLEAR(MOTOR_RIGHT_PWM_CW); GPIO_CLEAR(MOTOR_RIGHT_EN_CW); }
static void _motor_right_disable_ccw(void) { _motor_right_disconnect_pwm_ccw(); GPIO_CLEAR(MOTOR_RIGHT_PWM_CCW); GPIO_CLEAR(MOTOR_RIGHT_EN_CCW); }
static void _motor_left_enable_cw(void) { _motor_left_disable_ccw(); _delay_us(DEAD_TIME_US); _motor_left_connect_pwm_cw(); GPIO_SET(MOTOR_LEFT_EN_CW); }
static void _motor_left_enable_ccw(void) { _motor_left_disable_cw(); _delay_us(DEAD_TIME_US); _motor_left_connect_pwm_ccw(); GPIO_SET(MOTOR_LEFT_EN_CCW); }
static void _motor_right_enable_cw(void) { _motor_right_disable_ccw(); _delay_us(DEAD_TIME_US); _motor_right_connect_pwm_cw(); GPIO_SET(MOTOR_RIGHT_EN_CW); }
static void _motor_right_enable_ccw(void) { _motor_right_disable_cw(); _delay_us(DEAD_TIME_US); _motor_right_connect_pwm_ccw(); GPIO_SET(MOTOR_RIGHT_EN_CCW); }

static void _motor_left_set_pwm_cw(uint8_t speed) { OCR0B = speed; }
static void _motor_left_set_pwm_ccw(uint8_t speed) { OCR0A = speed; }
static void _motor_right_set_pwm_cw(uint8_t speed) { OCR1A = speed; }
static void _motor_right_set_pwm_ccw(uint8_t speed) { OCR1B = speed; }

static uint8_t _motor_left_direction = 0;  // 0 = stopped, 1 = cw, 2 = ccw
static uint8_t _motor_right_direction = 0;  // 0 = stopped, 1 = cw, 2 = ccw
static uint8_t _motor_left_old_speed = 0;
static uint8_t _motor_right_old_speed = 0;

void motor_init(void)
{
	GPIO_OUTPUT(MOTOR_LEFT_EN_CW); _motor_left_disable_cw();
	GPIO_OUTPUT(MOTOR_LEFT_EN_CCW); _motor_left_disable_ccw();
	GPIO_OUTPUT(MOTOR_LEFT_PWM_CW); _motor_left_set_pwm_cw(0);
	GPIO_OUTPUT(MOTOR_LEFT_PWM_CCW); _motor_left_set_pwm_ccw(0);
	_motor_left_config_pwm();

	GPIO_OUTPUT(MOTOR_RIGHT_EN_CW); _motor_right_disable_cw();
	GPIO_OUTPUT(MOTOR_RIGHT_EN_CCW); _motor_right_disable_ccw();
	GPIO_OUTPUT(MOTOR_RIGHT_PWM_CW); _motor_right_set_pwm_cw(0);
	GPIO_OUTPUT(MOTOR_RIGHT_PWM_CCW); _motor_right_set_pwm_ccw(0);
	_motor_right_config_pwm();
}

// -255 to 255 : negative = cw, positive = ccw, 0 = stop
void motor_left_set(int16_t speed)
{
	if (speed < -255 || speed > 255) speed = 0;  // out of range, stop the motor
	if (speed == _motor_left_old_speed) return;  // no change, do nothing
	if (speed == 0)
	{
		_motor_left_disable_cw(); _motor_left_disable_ccw();
		_motor_left_direction = 0; _motor_left_old_speed = 0;
		return;
	}
	if (speed > 0) // ccw
	{
		_motor_left_set_pwm_ccw(_motor_left_old_speed = speed);
		if (_motor_left_direction != 2) _motor_left_enable_ccw();
		_motor_left_direction = 2;
		return;
	} // else speed < 0, cw
	_motor_left_set_pwm_cw(_motor_left_old_speed = -speed);
	if (_motor_left_direction != 1) _motor_left_enable_cw();
	_motor_left_direction = 1;
}

// -255 to 255 : negative = ccw, positive = cw, 0 = stop
void motor_right_set(int16_t speed)
{
	if (speed < -255 || speed > 255) speed = 0;  // out of range, stop the motor
	if (speed == _motor_right_old_speed) return;  // no change, do nothing
	if (speed == 0)
	{
		_motor_right_disable_cw(); _motor_right_disable_ccw();
		_motor_right_direction = 0; _motor_right_old_speed = 0;
		return;
	}
	if (speed > 0) // cw
	{
		_motor_right_set_pwm_cw(_motor_right_old_speed = speed);
		if (_motor_right_direction != 1) _motor_right_enable_cw();
		_motor_right_direction = 1;
		return;
	} // else speed < 0, ccw
	_motor_right_set_pwm_ccw(_motor_right_old_speed = -speed);
	if (_motor_right_direction != 2) _motor_right_enable_ccw();
	_motor_right_direction = 2;
}
