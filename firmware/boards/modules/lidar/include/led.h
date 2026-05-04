#ifndef LED_H
# define LED_H

# include "utils.h"

# define LED_GREEN _PD6
# define LED_RED _PD7

/*
Strange thing here, don't work without inline
worked with code in main(), but not functions calls in main()
*/

inline void led_green_on(void) { GPIO_SET(LED_GREEN); }
inline void led_green_off(void) { GPIO_CLEAR(LED_GREEN); }
inline void led_green_toggle(void) { GPIO_TOGGLE(LED_GREEN); }
inline void led_red_on(void) { GPIO_SET(LED_RED); }
inline void led_red_off(void) { GPIO_CLEAR(LED_RED); }
inline void led_red_toggle(void) { GPIO_TOGGLE(LED_RED); }

inline void led_init(void) {
	GPIO_OUTPUT(LED_GREEN); led_green_off();
	GPIO_OUTPUT(LED_RED); led_red_off();
}

/* inline void led_init(void);
inline void led_green_on(void);
inline void led_green_off(void);
inline void led_green_toggle(void);
inline void led_red_on(void);
inline void led_red_off(void);
inline void led_red_toggle(void); */

#endif
