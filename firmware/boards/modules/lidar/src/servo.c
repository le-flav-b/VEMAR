#include "servo.h"

#define SERVO_TIMER_PRESCALER 8UL
#define SERVO_TIMER_TOP ((F_CPU / SERVO_TIMER_PRESCALER / SERVO_PWM_FREQ) - 1UL)
#define SERVO_TICKS_PER_US (F_CPU / SERVO_TIMER_PRESCALER / 1000000UL)

static void _servo_config_pwm(void)
{
	// Fast PWM, mode 14 (WGM13:0 = 1110), prescaler = 8
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
	TCCR1B = (1 << WGM13)  | (1 << WGM12)  | (1 << CS11);

	// Frequency configuration
	ICR1  = SERVO_TIMER_TOP;
}

static uint16_t _servo_us_to_ocr(uint16_t us) { return ((uint32_t)us * SERVO_TICKS_PER_US); }

static uint16_t _servo_deg_to_us(uint8_t deg)
{
    if (deg > SERVO_DEG_MAX) deg = SERVO_DEG_MAX;
    return (SERVO_US_MIN + (((SERVO_US_MAX - SERVO_US_MIN) * (uint32_t)deg) / SERVO_DEG_MAX));
}

static void _servo_a_set_us(uint16_t us)
{
    if (us < SERVO_US_MIN) us = SERVO_US_MIN;
    if (us > SERVO_US_MAX) us = SERVO_US_MAX;
    OCR1A = _servo_us_to_ocr(us);
}

static void _servo_b_set_us(uint16_t us)
{
    if (us < SERVO_US_MIN) us = SERVO_US_MIN;
    if (us > SERVO_US_MAX) us = SERVO_US_MAX;
    OCR1B = _servo_us_to_ocr(us);
}

void servo_init(void)
{
	SET_MSK(SERVO_DDR, MSK(SERVO_A_PIN) | MSK(SERVO_B_PIN));
	_servo_config_pwm();
}

void servo_a_set(uint8_t deg) { _servo_a_set_us(_servo_deg_to_us(deg)); }
void servo_b_set(uint8_t deg) { _servo_b_set_us(_servo_deg_to_us(deg)); }
