#ifndef LIDAR_H
# define LIDAR_H

# include "utils.h"

# define LIDAR_EN _PE4
# define LIDAR_PWM _PE3

# define LIDAR_MOTOR_PWM_FREQ 10000UL
# define LIDAR_MOTOR_PWM_DUTY_CYCLE 35

void lidar_init(void);
void lidar_on(void);
void lidar_off(void);

#endif
