#ifndef SERVO_H
# define SERVO_H

# include "utils.h"

# define SERVO_A _PB5
# define SERVO_B _PB6

# define SERVO_PWM_FREQ 50UL
# define SERVO_US_MIN 500UL
# define SERVO_US_MAX 2500UL
# define SERVO_DEG_MIN 0
# define SERVO_DEG_MAX 180

# define SERVO_DEG_REEL_MIN 45
# define SERVO_DEG_REEL_MAX 135
# define SERVO_MOUV_DEG_1 1
# define SERVO_MOUV_DEG_2 5


# define SERVO_TIMER_PRESCALER 8UL
# define SERVO_TIMER_TOP ((F_CPU / SERVO_TIMER_PRESCALER / SERVO_PWM_FREQ) - 1UL)
# define SERVO_TICKS_PER_US (F_CPU / SERVO_TIMER_PRESCALER / 1000000UL)

inline uint16_t _servo_us_to_ocr(uint16_t us) { return ((uint32_t)us * SERVO_TICKS_PER_US); }

inline uint16_t _servo_deg_to_us(uint8_t deg)
{
    if (deg > SERVO_DEG_MAX) deg = SERVO_DEG_MAX;
    return (SERVO_US_MIN + (((SERVO_US_MAX - SERVO_US_MIN) * (uint32_t)deg) / SERVO_DEG_MAX));
}

inline void _servo_a_set_us(uint16_t us)
{
    if (us < SERVO_US_MIN) us = SERVO_US_MIN;
    if (us > SERVO_US_MAX) us = SERVO_US_MAX;
    OCR1A = _servo_us_to_ocr(us);
}

inline void _servo_b_set_us(uint16_t us)
{
    if (us < SERVO_US_MIN) us = SERVO_US_MIN;
    if (us > SERVO_US_MAX) us = SERVO_US_MAX;
    OCR1B = _servo_us_to_ocr(us);
}

uint8_t _servo_a_pos;
uint8_t _servo_b_pos;

inline void _servo_update_pos(void)
{
	_servo_a_set_us(_servo_deg_to_us(_servo_a_pos));
	_servo_b_set_us(_servo_deg_to_us(_servo_b_pos));
}

inline void _servo_config_pwm(void)
{
	// Fast PWM, mode 14 (WGM13:0 = 1110), prescaler = 8
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
	TCCR1B = (1 << WGM13)  | (1 << WGM12)  | (1 << CS11);

	// Frequency configuration
	ICR1 = SERVO_TIMER_TOP;

	// Set to middle position
	_servo_a_pos = 90;
	_servo_b_pos = 90;
	_servo_update_pos();
}

inline void servo_init(void)
{
	GPIO_OUTPUT(SERVO_A);
	GPIO_OUTPUT(SERVO_B);
	_servo_config_pwm();
}

inline void servo_a_l1(void) { _servo_a_pos -= SERVO_MOUV_DEG_1; if (_servo_a_pos < SERVO_DEG_REEL_MIN) _servo_a_pos = SERVO_DEG_REEL_MIN; _servo_update_pos(); }
inline void servo_a_l2(void) { _servo_a_pos -= SERVO_MOUV_DEG_2; if (_servo_a_pos < SERVO_DEG_REEL_MIN) _servo_a_pos = SERVO_DEG_REEL_MIN; _servo_update_pos(); }
inline void servo_a_r1(void) { _servo_a_pos += SERVO_MOUV_DEG_1; if (_servo_a_pos > SERVO_DEG_REEL_MAX) _servo_a_pos = SERVO_DEG_REEL_MAX; _servo_update_pos(); }
inline void servo_a_r2(void) { _servo_a_pos += SERVO_MOUV_DEG_2; if (_servo_a_pos > SERVO_DEG_REEL_MAX) _servo_a_pos = SERVO_DEG_REEL_MAX; _servo_update_pos(); }
inline void servo_b_l1(void) { _servo_b_pos -= SERVO_MOUV_DEG_1; if (_servo_b_pos < SERVO_DEG_REEL_MIN) _servo_b_pos = SERVO_DEG_REEL_MIN; _servo_update_pos(); }
inline void servo_b_l2(void) { _servo_b_pos -= SERVO_MOUV_DEG_2; if (_servo_b_pos < SERVO_DEG_REEL_MIN) _servo_b_pos = SERVO_DEG_REEL_MIN; _servo_update_pos(); }
inline void servo_b_r1(void) { _servo_b_pos += SERVO_MOUV_DEG_1; if (_servo_b_pos > SERVO_DEG_REEL_MAX) _servo_b_pos = SERVO_DEG_REEL_MAX; _servo_update_pos(); }
inline void servo_b_r2(void) { _servo_b_pos += SERVO_MOUV_DEG_2; if (_servo_b_pos > SERVO_DEG_REEL_MAX) _servo_b_pos = SERVO_DEG_REEL_MAX; _servo_update_pos(); }

#endif
