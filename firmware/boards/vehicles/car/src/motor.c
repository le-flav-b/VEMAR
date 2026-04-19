#include "motor.h"

static void _motor_left_config_pwm(void)
{
	SET_MSK(TCCR0A, MSK(WGM01) | MSK(WGM00) | MSK(COM0A1) | MSK(COM0B1));  // Fast PWM, non-inverting
	SET_MSK(TCCR0B, MSK(CS01));  // prescaler = 8
}

static void _motor_right_config_pwm(void)
{
	SET_MSK(TCCR1A, MSK(WGM10) | MSK(COM1A1) | MSK(COM1B1));  // Mode 5 (8-bit fast PWM), non-inverting
	SET_MSK(TCCR1B, MSK(WGM12) | MSK(CS11));  // prescaler = 8
}

static void _motor_left_enable_cw(void) { GPIO_SET(MOTOR_LEFT_EN_CW); }
static void _motor_left_disable_cw(void) { GPIO_CLEAR(MOTOR_LEFT_EN_CW); }
static void _motor_left_enable_ccw(void) { GPIO_SET(MOTOR_LEFT_EN_CCW); }
static void _motor_left_disable_ccw(void) { GPIO_CLEAR(MOTOR_LEFT_EN_CCW); }
static void _motor_right_enable_cw(void) { GPIO_SET(MOTOR_RIGHT_EN_CW); }
static void _motor_right_disable_cw(void) { GPIO_CLEAR(MOTOR_RIGHT_EN_CW); }
static void _motor_right_enable_ccw(void) { GPIO_SET(MOTOR_RIGHT_EN_CCW); }
static void _motor_right_disable_ccw(void) { GPIO_CLEAR(MOTOR_RIGHT_EN_CCW); }
static void _motor_left_set_pwm_cw(uint8_t speed) { OCR0B = speed; }
static void _motor_left_set_pwm_ccw(uint8_t speed) { OCR0A = speed; }
static void _motor_right_set_pwm_cw(uint8_t speed) { OCR1A = speed; }
static void _motor_right_set_pwm_ccw(uint8_t speed) { OCR1B = speed; }

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

/* TODO add easy interface with protection
- deadtime 5us
- maybe a mapping if 0-255 is not linear with speed
- static variables to track state
*/


/*
void setup() {
  pinMode(PIN_M1_EN_CW, OUTPUT);
  pinMode(PIN_M1_EN_CCW, OUTPUT);
  pinMode(PIN_M1_PWM_CW, OUTPUT);
  pinMode(PIN_M1_PWM_CCW, OUTPUT);

  pinMode(PIN_M2_EN_CW, OUTPUT);
  pinMode(PIN_M2_EN_CCW, OUTPUT);
  pinMode(PIN_M2_PWM_CW, OUTPUT);
  pinMode(PIN_M2_PWM_CCW, OUTPUT);

  off();
}

void off() {
  digitalWrite(PIN_M1_EN_CW, LOW);
  digitalWrite(PIN_M1_PWM_CW, LOW);
  digitalWrite(PIN_M1_EN_CCW, LOW);
  digitalWrite(PIN_M1_PWM_CCW, LOW);
  digitalWrite(PIN_M2_EN_CW, LOW);
  digitalWrite(PIN_M2_PWM_CW, LOW);
  digitalWrite(PIN_M2_EN_CCW, LOW);
  digitalWrite(PIN_M2_PWM_CCW, LOW);
  delay(2); // 5us dead time needed between switching directions
}


void on() {
  digitalWrite(PIN_M1_EN_CW, HIGH);
  analogWrite(PIN_M1_PWM_CW, 255);
  digitalWrite(PIN_M2_EN_CW, HIGH);
  analogWrite(PIN_M2_PWM_CW, 255);
}

void loop() {
  digitalWrite(PIN_M1_EN_CW, HIGH);
  analogWrite(PIN_M1_PWM_CW, 100);
  digitalWrite(PIN_M2_EN_CW, HIGH);
  analogWrite(PIN_M2_PWM_CW, 100);
  delay(5000);

  off();
  delay(2000);

  digitalWrite(PIN_M1_EN_CW, HIGH);
  analogWrite(PIN_M1_PWM_CW, 255);
  digitalWrite(PIN_M2_EN_CW, HIGH);
  analogWrite(PIN_M2_PWM_CW, 255);
  delay(5000);

  off();
  delay(2000);

  digitalWrite(PIN_M1_EN_CCW, HIGH);
  analogWrite(PIN_M1_PWM_CCW, 100);
  digitalWrite(PIN_M2_EN_CCW, HIGH);
  analogWrite(PIN_M2_PWM_CCW, 100);
  delay(5000);

  off();
  delay(2000);

  digitalWrite(PIN_M1_EN_CCW, HIGH);
  analogWrite(PIN_M1_PWM_CCW, 255);
  digitalWrite(PIN_M2_EN_CCW, HIGH);
  analogWrite(PIN_M2_PWM_CCW, 255);
  delay(5000);

  off();
  delay(2000);
}
*/
