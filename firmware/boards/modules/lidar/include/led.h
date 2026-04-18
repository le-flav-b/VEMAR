#ifndef LED_H
# define LED_H

# include "utils.h"

# define LEDS_DDR DDRD
# define LEDS_PORT PORTD
# define LED_GREEN_PIN PD6
# define LED_RED_PIN PD7

void led_init(void);
void led_green_on(void);
void led_green_off(void);
void led_green_toggle(void);
void led_red_on(void);
void led_red_off(void);
void led_red_toggle(void);

#endif
