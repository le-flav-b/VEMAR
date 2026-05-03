#ifndef MOTOR_H
# define MOTOR_H

# include "utils.h"

#define MOTOR_LEFT_EN_CW _PD7
#define MOTOR_LEFT_EN_CCW _PB0
#define MOTOR_LEFT_PWM_CW _PD5
#define MOTOR_LEFT_PWM_CCW _PD6

#define MOTOR_RIGHT_EN_CW _PC0
#define MOTOR_RIGHT_EN_CCW _PC1
#define MOTOR_RIGHT_PWM_CW _PB1
#define MOTOR_RIGHT_PWM_CCW _PB2

#define DEAD_TIME_US    5000UL
#define MOTOR_RAMP_STEP 16      // PWM units per ramp step
#define MOTOR_RAMP_MS   4       // ms per ramp step (~64ms for full-speed reversal)

void motor_init(void);
void motor_left_set(int16_t speed);
void motor_right_set(int16_t speed);

#endif
