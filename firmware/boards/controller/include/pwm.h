#ifndef VEMAR_PWM_H
#define VEMAR_PWM_H

#include "gpio.h"

typedef enum
{
    PWM_0A = PIN_PD6,
    PWM_0B = PIN_PD5,
    PWM_1A = PIN_PB1,
    PWM_1B = PIN_PB2,
    PWM_2A = PIN_PB3,
    PWM_2B = PIN_PD3
} pwm_t;

void PWM_init(pwm_t pin);

void PWM_write(pwm_t pin, uint8_t value);

void PWM_enable(pwm_t pin);

void PWM_disable(pwm_t pin);

#endif // VEMAR_PWM_H
