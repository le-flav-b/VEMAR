#ifndef GPIO_H
# define GPIO_H

typedef struct {
	volatile uint8_t	*ddr;
	volatile uint8_t	*port;
	volatile uint8_t	*pin;
	uint8_t				bit;
}					gpio_t;

# define _PB0 ((gpio_t){&DDRB, &PORTB, &PINB, PB0})
# define _PB1 ((gpio_t){&DDRB, &PORTB, &PINB, PB1})
# define _PB2 ((gpio_t){&DDRB, &PORTB, &PINB, PB2})
# define _PB3 ((gpio_t){&DDRB, &PORTB, &PINB, PB3})
# define _PB4 ((gpio_t){&DDRB, &PORTB, &PINB, PB4})
# define _PB5 ((gpio_t){&DDRB, &PORTB, &PINB, PB5})
# define _PB6 ((gpio_t){&DDRB, &PORTB, &PINB, PB6})
# define _PB7 ((gpio_t){&DDRB, &PORTB, &PINB, PB7})

# define _PC0 ((gpio_t){&DDRC, &PORTC, &PINC, PC0})
# define _PC1 ((gpio_t){&DDRC, &PORTC, &PINC, PC1})
# define _PC2 ((gpio_t){&DDRC, &PORTC, &PINC, PC2})
# define _PC3 ((gpio_t){&DDRC, &PORTC, &PINC, PC3})
# define _PC4 ((gpio_t){&DDRC, &PORTC, &PINC, PC4})
# define _PC5 ((gpio_t){&DDRC, &PORTC, &PINC, PC5})
# define _PC6 ((gpio_t){&DDRC, &PORTC, &PINC, PC6})
# define _PC7 ((gpio_t){&DDRC, &PORTC, &PINC, PC7})

# define _PD0 ((gpio_t){&DDRD, &PORTD, &PIND, PD0})
# define _PD1 ((gpio_t){&DDRD, &PORTD, &PIND, PD1})
# define _PD2 ((gpio_t){&DDRD, &PORTD, &PIND, PD2})
# define _PD3 ((gpio_t){&DDRD, &PORTD, &PIND, PD3})
# define _PD4 ((gpio_t){&DDRD, &PORTD, &PIND, PD4})
# define _PD5 ((gpio_t){&DDRD, &PORTD, &PIND, PD5})
# define _PD6 ((gpio_t){&DDRD, &PORTD, &PIND, PD6})
# define _PD7 ((gpio_t){&DDRD, &PORTD, &PIND, PD7})

#endif
