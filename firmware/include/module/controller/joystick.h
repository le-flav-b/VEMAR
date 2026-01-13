#ifndef VEMAR_JOYSTICK_H
#define VEMAR_JOYSTICK_H

#include "common.h"

/**
 * @defgroup joy_analog Joystick Analog Pin
 * @brief Available analog pin selection for X and Y
 * @see JOYSTICK_init
 * @see JOYSTICK_read
 * @{
 */
#define JOYSTICK_ANALOG_0 0 ///< Mapped to PC0
#define JOYSTICK_ANALOG_1 1 ///< Mapped to PC1
#define JOYSTICK_ANALOG_2 2 ///< Mapped to PC2
#define JOYSTICK_ANALOG_3 3 ///< Mapped to PC3
#define JOYSTICK_ANALOG_4 4 ///< Mapped to PC4
#define JOYSTICK_ANALOG_5 5 ///< Mapped to PC5
/**
 * @}
 */

/**
 * @defgroup joy_button Joystick Button Pin
 * @brief Available pin selection for the button
 * @see JOYSTICK_init
 * @see JOYSTICK_read
 * @{
 */
#define JOYSTICK_BUTTON_0 5 ///< Mapped to PD5
#define JOYSTICK_BUTTON_1 6 ///< Mapped to PD6
#define JOYSTICK_BUTTON_2 7 ///< Mapped to PD7
/**
 * @}
 */

/**
 * @struct joystick
 * @brief Define structure of a Joystick
 */
typedef struct joystick
{
    unsigned int x; ///< Value on X axis
    unsigned int y; ///< Value on Y axis
    struct
    {
        unsigned char pressed; ///< State of the button
    } button;                  ///< Button
} joystick_t;                  ///< Alias to `struct joystick`

/**
 * @brief Initialize a joystick by configuring pins as inputs
 * @param x Analog pin X
 * @param y Analog pin Y
 * @param btn Digital pin of the button
 * @see joy_analog
 * @see joy_button
 */
void JOYSTICK_init(byte_t x, byte_t y, byte_t btn);

/**
 * @brief Read the values from the joystick
 * @param joy Pointer to the joystick structure to store the result
 * @param x Analog pin X
 * @param y Analog pin Y
 * @param btn Digital pin of the button
 * @see joy_analog
 * @see joy_button
 */
void JOYSTICK_read(struct joystick *joy, byte_t x, byte_t y, byte_t btn);

#endif // VEMAR_JOYSTICK

/**
 * @file joystick.h
 * @brief Joystick manipulation
 * @author Christian Hugon
 * @version 0.0.1
 */
