#ifndef VEMAR_JOYSTICK_H
#define VEMAR_JOYSTICK_H

#include "common.h"
#include "gpio.h"

/**
 * @brief Define structure of a Joystick
 */
typedef struct
{
    analog_t x;      /**< X-axis */
    analog_t y;      /**< Y-axis */
    button_t button; /**< Button */
} joystick_t;

/**
 * @brief Instanciate a new Joystick
 * @param x Pin of the Joystick X-axis
 * @param y Pin of the Joystick Y-axis
 * @param sw Pin of the Joystick button
 * @return Joystick structure
 */
joystick_t JOYSTICK_new(adc_ch_t x, adc_ch_t y, pin_t sw);

/**
 * @brief Return the value of the X-axis
 * @param joystick Pointer to the Joystick structure to read
 * @return Value of the X-axis
 */
inline uint16_t JOYSTICK_x(joystick_t *joystick)
{
    return (ANALOG_read(joystick->x));
}

/**
 * @brief Return the value of the Y-axis
 * @param joystick Pointer to the Joystick structure to read
 * @return Value of the Y-axis
 */
inline uint16_t JOYSTICK_y(joystick_t *joystick)
{
    return (ANALOG_read(joystick->y));
}

/**
 * @brief Check whether the Joystick is pressed
 * @param joystick Pointer to the Joystick structure to check
 * @return `TRUE` if the Joystick is pressed, otherwise `FALSE`
 */
inline bool_t JOYSTICK_is_pressed(joystick_t *joystick)
{
    return (BUTTON_is_active(&(joystick->button)));
}

#endif // VEMAR_JOYSTICK

/**
 * @file joystick.h
 * @brief Joystick utility functions
 * @author Christian Hugon <chriss.hugon@gmail.com>
 * @version 1.0.0
 */
