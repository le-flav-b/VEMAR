#ifndef VEMAR_PWM_H
#define VEMAR_PWM_H

#include "gpio.h"

/**
 * @brief Define PWM Pin
 */
typedef enum
{
    PWM_0A = PIN_PD6, /**< Pin OCR0A */
    PWM_0B = PIN_PD5, /**< Pin OCR0B */
    PWM_1A = PIN_PB1, /**< Pin OCR1A */
    PWM_1B = PIN_PB2, /**< Pin OCR1B */
    PWM_2A = PIN_PB3, /**< Pin OCR2A */
    PWM_2B = PIN_PD3  /**< Pin OCR2B */
} pwm_t;

/**
 * @brief Initialize PWM
 * @brief pin PWM Pin
 * @see pwm_t
 */
void PWM_init(pwm_t pin);

/**
 * @brief Set duty cycle
 * @brief pin PWM Pin
 * @brief value Duty cycle [0:255]
 */
void PWM_write(pwm_t pin, uint8_t value);

/**
 * @brief Enable PWM
 */
void PWM_enable(pwm_t pin);

/**
 * @brief Disable PWM
 */
void PWM_disable(pwm_t pin);

#endif // VEMAR_PWM_H

/**
 * @file pwm.h
 * @brief Utility functions for PWM
 * @author Christian Hugon <chriss.hugon@gmail.com>
 */
