#ifndef LED_H
# define LED_H

# include "utils.h"

# define LED_GREEN _PD6
# define LED_RED _PD7

void led_init(void);
void led_green_on(void);
void led_green_off(void);
void led_green_toggle(void);
void led_red_on(void);
void led_red_off(void);
void led_red_toggle(void);

#endif
