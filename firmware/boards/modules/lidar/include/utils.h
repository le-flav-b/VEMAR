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

# define SET_INPUT(DDRx, pin) CLEAR(DDRx, pin)
# define SET_OUTPUT(DDRx, pin) SET(DDRx, pin)

# define GET_VALUE(reg, pin) ((reg >> pin) & 1)

#endif
