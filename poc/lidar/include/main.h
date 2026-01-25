#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>


# ifndef UART_BAUDRATE
#  define UART_BAUDRATE 115200
# endif


# define MSK(bit) (1 << bit)

# define SET_MSK(reg, msk) reg |= (msk)
# define CLEAR_MSK(reg, msk) reg &= ~(msk)

# define SET(reg, pin) SET_MSK(reg, MSK(pin))
# define CLEAR(reg, pin) CLEAR_MSK(reg, MSK(pin))

# define SET_INPUT(DDRx, pin) CLEAR(DDRx, pin)
# define SET_OUTPUT(DDRx, pin) SET(DDRx, pin)

# define SET_ON(PORTx, pin) SET(PORTx, pin)
# define SET_OFF(PORTx, pin) CLEAR(PORTx, pin)
# define TOGGLE(PORTx, pin) PORTx ^= (MSK(pin))

# define GET_VALUE(reg, pin) ((reg >> pin) & 1)
# define IS_PRESSED(PINx, pin) !GET_VALUE(PINx, pin)

# define DEBOUNCE _delay_ms(2)

#endif
