#ifndef GPIO_H
# define GPIO_H

typedef struct {
	volatile uint8_t	*ddr;
	volatile uint8_t	*port;
	volatile uint8_t	*pin;
	uint8_t				bit;
}					gpio_t;

# define _PA0 ((gpio_t){&DDRA, &PORTA, &PINA, PA0})
# define _PA1 ((gpio_t){&DDRA, &PORTA, &PINA, PA1})
# define _PA2 ((gpio_t){&DDRA, &PORTA, &PINA, PA2})
# define _PA3 ((gpio_t){&DDRA, &PORTA, &PINA, PA3})
# define _PA4 ((gpio_t){&DDRA, &PORTA, &PINA, PA4})
# define _PA5 ((gpio_t){&DDRA, &PORTA, &PINA, PA5})
# define _PA6 ((gpio_t){&DDRA, &PORTA, &PINA, PA6})
# define _PA7 ((gpio_t){&DDRA, &PORTA, &PINA, PA7})

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

# define _PE0 ((gpio_t){&DDRE, &PORTE, &PINE, PE0})
# define _PE1 ((gpio_t){&DDRE, &PORTE, &PINE, PE1})
# define _PE2 ((gpio_t){&DDRE, &PORTE, &PINE, PE2})
# define _PE3 ((gpio_t){&DDRE, &PORTE, &PINE, PE3})
# define _PE4 ((gpio_t){&DDRE, &PORTE, &PINE, PE4})
# define _PE5 ((gpio_t){&DDRE, &PORTE, &PINE, PE5})
# define _PE6 ((gpio_t){&DDRE, &PORTE, &PINE, PE6})
# define _PE7 ((gpio_t){&DDRE, &PORTE, &PINE, PE7})

# define _PF0 ((gpio_t){&DDRF, &PORTF, &PINF, PF0})
# define _PF1 ((gpio_t){&DDRF, &PORTF, &PINF, PF1})
# define _PF2 ((gpio_t){&DDRF, &PORTF, &PINF, PF2})
# define _PF3 ((gpio_t){&DDRF, &PORTF, &PINF, PF3})
# define _PF4 ((gpio_t){&DDRF, &PORTF, &PINF, PF4})
# define _PF5 ((gpio_t){&DDRF, &PORTF, &PINF, PF5})
# define _PF6 ((gpio_t){&DDRF, &PORTF, &PINF, PF6})
# define _PF7 ((gpio_t){&DDRF, &PORTF, &PINF, PF7})

# define _PG0 ((gpio_t){&DDRG, &PORTG, &PING, PG0})
# define _PG1 ((gpio_t){&DDRG, &PORTG, &PING, PG1})
# define _PG2 ((gpio_t){&DDRG, &PORTG, &PING, PG2})
# define _PG3 ((gpio_t){&DDRG, &PORTG, &PING, PG3})
# define _PG4 ((gpio_t){&DDRG, &PORTG, &PING, PG4})
# define _PG5 ((gpio_t){&DDRG, &PORTG, &PING, PG5})
# define _PG6 ((gpio_t){&DDRG, &PORTG, &PING, PG6})
# define _PG7 ((gpio_t){&DDRG, &PORTG, &PING, PG7})

#endif
