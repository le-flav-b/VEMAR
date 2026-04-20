#include "buzzer.h"

static void _buzzer_config_pwm(void)
{
	TCCR2A = 0; SET_MSK(TCCR2A, MSK(WGM21)); // CTC mode, OCR2A as TOP
	SET_MSK(TIMSK2, MSK(OCIE2A)); // Enable interrupt
	sei();
}

ISR(TIMER2_COMPA_vect) { GPIO_TOGGLE(BUZZER); }

void buzzer_init(void)
{
	GPIO_OUTPUT(BUZZER); GPIO_CLEAR(BUZZER);
	_buzzer_config_pwm();
}

void buzzer_off(void)
{
	CLEAR_MSK(TCCR2B, MSK(CS22) | MSK(CS20));
	GPIO_CLEAR(BUZZER);
}

void buzzer_tone(uint8_t value)
{
	OCR2A = value;
	SET_MSK(TCCR2B, MSK(CS22) | MSK(CS20));
}
