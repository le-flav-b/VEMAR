#include "buzzer.h"

static void _buzzer_config_pwm(void)
{
	SET_MSK(TCCR2A, MSK(WGM21)); // CTC mode, OCR2A as TOP
	SET_MSK(TIMSK2, MSK(OCIE2A)); // Enable interrupt
}

ISR(TIMER2_COMPA_vect) { GPIO_TOGGLE(BUZZER); }

void buzzer_init(void)
{
	GPIO_OUTPUT(BUZZER); buzzer_off();
	_buzzer_config_pwm();
}

void buzzer_off(void)
{
	CLEAR_MSK(TCCR2B, MSK(CS22) | MSK(CS21) | MSK(CS20));
	GPIO_CLEAR(BUZZER);
}

// Threasholds vibe calculated for F_CPU = 16 MHz
static uint16_t _buzzer_pick_prescaler(uint16_t f)
{
	CLEAR_MSK(TCCR2B, MSK(CS22) | MSK(CS21) | MSK(CS20));
	if (f >= 3906U) { SET_MSK(TCCR2B, MSK(CS21)); return 8; }
	if (f >= 976U) { SET_MSK(TCCR2B, MSK(CS21) | MSK(CS20)); return 32; }
	if (f >= 488U) { SET_MSK(TCCR2B, MSK(CS22)); return 64; }
	if (f >= 244U) { SET_MSK(TCCR2B, MSK(CS22) | MSK(CS20)); return 128; }
	if (f >= 122U) { SET_MSK(TCCR2B, MSK(CS22) | MSK(CS21)); return 256; }
	SET_MSK(TCCR2B, MSK(CS22) | MSK(CS21) | MSK(CS20)); return 1024;
}

void buzzer_tone(uint16_t frequency)
{
    if (frequency < 100U || frequency > 10000U) return;
    OCR2A = (uint8_t)((F_CPU / (2UL * _buzzer_pick_prescaler(frequency) * (uint32_t)frequency)) - 1UL);
}
