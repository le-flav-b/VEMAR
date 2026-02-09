#ifndef VEMAR_JOYSTICK_H
#define VEMAR_JOYSTICK_H

#include "common.h"
#include "gpio.h"

/**
 * @struct joystick
 * @brief Define structure of a Joystick
 */
typedef struct vemar_struct_joystick
{
	analog_t x;
	analog_t y;
	button_t button;
} joystick_t;

/**
 * @brief Instanciate a new Joystick
 * @param x Pin of the Joystick X-axis
 * @param y Pin of the Joystick Y-axis
 * @param sw Pin of the Joystick button
 */
joystick_t JOYSTICK_new(pin_t x, pin_t y, pin_t sw);

/**
 * @brief Return the value of the X-axis
 * @param joystick Pointer to the Joystick structure to read
 * @return Value of the X-axis
 */
inline unsigned int JOYSTICK_x(joystick_t *joystick)
{
	return (ANALOG_read(joystick->x));
}

/**
 * @brief Return the value of the Y-axis
 * @param joystick Pointer to the Joystick structure to read
 * @return Value of the Y-axis
 */
inline unsigned int JOYSTICK_y(joystick_t *joystick)
{
	return (ANALOG_read(joystick->y));
}

/**
 * @brief Check whether the Joystick is pressed
 * @param joystick Pointer to the Joystick structure to check
 * @return Non-zero value if the Joystick is pressed, otherwise `0`
 */
inline bool_t JOYSTICK_is_pressed(joystick_t *joystick)
{
	return (BUTTON_is_active(&(joystick->button)));
}

#endif // VEMAR_JOYSTICK

/**
 * @file joystick.h
 * @brief Joystick manipulation
 * @author Christian Hugon
 * @version 0.0.1
 */
