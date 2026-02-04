#ifndef VEMAR_LED_H
#define VEMAR_LED_H

#include "common.h"
#include "pin.h"

/**
 * @brief LED
 */
typedef struct led
{
	byte_t pin;
} led_t;

inline led_t LED_new(byte_t pin)
{
	led_t retval = {.pin = pin};

	PIN_mode(pin, PIN_OUTPUT);
	return (retval);
}

inline void LED_on(led_t led)
{
	PIN_write(led.pin, PIN_HIGH);
}

inline void LED_off(led_t led)
{
	PIN_write(led.pin, PIN_LOW);
}

inline void LED_toggle(led_t led)
{
	PIN_toggle(led.pin);
}

inline bool_t LED_ison(led_t led)
{
	return (PIN_HIGH == PIN_read(led.pin));
}

#endif // VEMAR_LED_H
