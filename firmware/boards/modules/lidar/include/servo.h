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

void servo_init(void);
void servo_a_set(uint8_t deg);
void servo_b_set(uint8_t deg);

#endif
