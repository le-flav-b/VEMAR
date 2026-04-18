#ifndef UTILS_H
# define UTILS_H

// Includes

# include <avr/io.h>
# include <util/delay.h>


// Bytes Management

# define MSK(bit) (1 << bit)

# define SET_MSK(reg, msk) reg |= msk
# define CLEAR_MSK(reg, msk) reg &= ~msk
# define TOGGLE_MSK(reg, msk) reg ^= msk

# define SET(reg, pin) SET_MSK(reg, MSK(pin))
# define CLEAR(reg, pin) CLEAR_MSK(reg, MSK(pin))
# define TOGGLE(reg, pin) TOGGLE_MSK(reg, MSK(pin))

# define GET_VALUE(reg, pin) ((reg >> pin) & 1)


// GPIO Management

# include "gpio.h"

# define GPIO_SET(gpio) SET(*gpio.port, gpio.bit)
# define GPIO_CLEAR(gpio) CLEAR(*gpio.port, gpio.bit)
# define GPIO_TOGGLE(gpio) TOGGLE(*gpio.port, gpio.bit)

# define GPIO_INPUT(gpio) CLEAR(*gpio.ddr, gpio.bit)
# define GPIO_OUTPUT(gpio) SET(*gpio.ddr, gpio.bit)

# define GPIO_GET(gpio) GET_VALUE(*gpio.pin, gpio.bit)

#endif
