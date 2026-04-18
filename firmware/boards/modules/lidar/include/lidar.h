#ifndef LIDAR_H
# define LIDAR_H

# include "utils.h"

# define LIDAR_DDR DDRE
# define LIDAR_PORT PORTE
# define LIDAR_EN_PIN PE4
# define LIDAR_PWM_PIN PE3

# define LIDAR_MOTOR_PWM_FREQ 10000UL
# define LIDAR_MOTOR_PWM_DUTY_CYCLE 35

void lidar_init(void);
void lidar_on(void);
void lidar_off(void);

#endif
