#include "led.h"

extern inline led_t LED_new(byte_t pin);
extern inline void LED_on(led_t led);
extern inline void LED_off(led_t led);
extern inline void LED_toggle(led_t led);
extern inline bool_t LED_ison(led_t led);
