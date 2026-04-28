#include "pwm.h"
#include "timer.h"

void PWM_init(pwm_t pin)
{
    PIN_mode(pin, PIN_OUTPUT);
    switch (pin)
    {
    case PWM_0A:
        TIMER0_init(TIMER0_FPWM, TIMER0_PS64);
        BIT_set(TCCR0A, BIT(COM0A1));
        break;
    case PWM_0B:
        TIMER0_init(TIMER0_FPWM, TIMER0_PS64);
        BIT_set(TCCR0A, BIT(COM0B1));
        break;
    case PWM_1A:
        TIMER1_init(TIMER1_FPWM8, TIMER1_PS64);
        BIT_set(TCCR1A, BIT(COM1A1));
        break;
    case PWM_1B:
        TIMER1_init(TIMER1_FPWM8, TIMER1_PS64);
        BIT_set(TCCR1A, BIT(COM1B1));
        break;
    case PWM_2A:
        TIMER2_init(TIMER2_FPWM, TIMER2_PS64);
        BIT_set(TCCR2A, BIT(COM2A1));
        break;
    case PWM_2B:
        TIMER2_init(TIMER2_FPWM, TIMER2_PS64);
        BIT_set(TCCR2A, BIT(COM2B1));
        break;
    default:
        break;
    }
}

void PWM_write(pwm_t pin, uint8_t value)
{
    switch (pin)
    {
    case PWM_0A:
        OCR0A = value;
        break;
    case PWM_0B:
        OCR0B = value;
        break;
    case PWM_1A:
        OCR1A = value;
        break;
    case PWM_1B:
        OCR1B = value;
        break;
    case PWM_2A:
        OCR2A = value;
        break;
    case PWM_2B:
        OCR2B = value;
        break;
    default:
        break;
    }
}

void PWM_disable(pwm_t pin)
{
    switch (pin)
    {
    case PWM_0A:
        BIT_clear(TCCR0A, (BIT(COM0A1) | BIT(COM0A0)));
        break;
    case PWM_0B:
        BIT_clear(TCCR0A, (BIT(COM0B1) | BIT(COM0B0)));
        break;
    case PWM_1A:
        BIT_clear(TCCR1A, (BIT(COM1A1) | BIT(COM1A0)));
        break;
    case PWM_1B:
        BIT_clear(TCCR1A, (BIT(COM1B1) | BIT(COM1B0)));
        break;
    case PWM_2A:
        BIT_clear(TCCR2A, (BIT(COM2A1) | BIT(COM2A0)));
        break;
    case PWM_2B:
        BIT_clear(TCCR2A, (BIT(COM2B1) | BIT(COM2B0)));
        break;
    default:
        break;
    }
    PIN_write(pin, PIN_LOW);
}

void PWM_enable(pwm_t pin)
{
    switch (pin)
    {
    case PWM_0A:
        BIT_set(TCCR0A, BIT(COM0A1));
        break;
    case PWM_0B:
        BIT_set(TCCR0A, BIT(COM0B1));
        break;
    case PWM_1A:
        BIT_set(TCCR1A, BIT(COM1A1));
        break;
    case PWM_1B:
        BIT_set(TCCR1A, BIT(COM1B1));
        break;
    case PWM_2A:
        BIT_set(TCCR2A, BIT(COM2A1));
        break;
    case PWM_2B:
        BIT_set(TCCR2A, BIT(COM2B1));
        break;
    default:
        break;
    }
    PIN_write(pin, PIN_LOW);
}
