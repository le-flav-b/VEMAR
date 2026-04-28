#ifndef VEMAR_TIMER_H
#define VEMAR_TIMER_H

#include "common.h"

/**
 * @brief Define Timer 0 Prescaler
 */
typedef enum
{
    TIMER0_PS1 = 0x01,
    TIMER0_PS8 = 0x02,
    TIMER0_PS64 = 0x03,
    TIMER0_PS256 = 0x04,
    TIMER0_PS1024 = 0x05
} timer0_ps_t;

/**
 * @brief Define Timer 1 Prescaler
 */
typedef enum
{
    TIMER1_PS1 = 0x01,
    TIMER1_PS8 = 0x02,
    TIMER1_PS64 = 0x03,
    TIMER1_PS256 = 0x04,
    TIMER1_PS1024 = 0x05
} timer1_ps_t;

typedef enum
{
    TIMER2_PS1 = 0x01,
    TIMER2_PS8 = 0x02,
    TIMER2_PS32 = 0x03,
    TIMER2_PS64 = 0x04,
    TIMER2_PS128 = 0x05,
    TIMER2_PS256 = 0x06,
    TIMER2_PS1024 = 0x07
} timer2_ps_t;

/**
 * @brief Define Timer Operation Mode
 */
typedef enum
{
    TIMER0_NORMAL = 0x00, /**< Normal mode */
    TIMER0_CTC = 0x01,    /**< Clear Timer on Compare Match mode */
    TIMER0_FPWM = 0x03     /**< Fast Pulse Width Modulation mode */
} timer0_mode_t;

typedef enum
{
    TIMER1_NORMA = 0x00,
    TIMER1_CTC = 0x08,
    TIMER1_FPWM8 = 0x09, /**< Fast Pulse Width Modulation 8-bit mode */
} timer1_mode_t;

typedef enum
{
    TIMER2_NORMAL = 0x00,
    TIMER2_CTC = 0x10,
    TIMER2_FPWM = 0x03
} timer2_mode_t;

/**
 * @brief Initialize Timer0
 * @param mode
 * @param prescaler
 */
void TIMER0_init(timer0_mode_t mode, timer0_ps_t prescaler);

/**
 * @brief Initialize Timer1
 * @param mode
 * @param prescaler
 */
void TIMER1_init(timer1_mode_t mode, timer1_ps_t prescaler);

/**
 * @brief Initialize Timer2
 * @param mode
 * @param prescaler
 */
void TIMER2_init(timer2_mode_t mode, timer2_ps_t prescaler);

#endif // VEMAR_TIMER_H

/**
 * @file timer.h
 * @brief Utility functions for timers
 * @author Christian Hugon <chriss.hugon@gmail.com>
 */
