#ifndef LIDAR_H
# define LIDAR_H

# include "utils.h"

# define LIDAR_EN _PE4
# define LIDAR_RST _PE3

inline void lidar_on(void) { GPIO_SET(LIDAR_EN); }
inline void lidar_off(void) { GPIO_CLEAR(LIDAR_EN); }
inline void lidar_reset(void) { GPIO_CLEAR(LIDAR_RST); _delay_ms(10); GPIO_SET(LIDAR_RST); }

volatile uint8_t _lidar_ready;

inline void lidar_init(void)
{
	GPIO_OUTPUT(LIDAR_EN); lidar_off();
	GPIO_OUTPUT(LIDAR_RST); GPIO_SET(LIDAR_RST);
	_lidar_ready = 0;
}

inline void lidar_take_mesurement(void)
{
	_lidar_ready = 0;
	// TODO
}

inline uint8_t lidar_is_ready(void) { return _lidar_ready; }

#endif
