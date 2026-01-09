#ifndef VEMAR_JOYSTICK_H
#define VEMAR_JOYSTICK_H

#include "common.h"

// Joystick Analog selection for X and Y
#define JOYSTICK_ANALOG_0 0 ///< Mapped to PC0
#define JOYSTICK_ANALOG_1 1 ///< Mapped to PC1
#define JOYSTICK_ANALOG_2 2 ///< Mapped to PC2
#define JOYSTICK_ANALOG_3 3 ///< Mapped to PC3
#define JOYSTICK_ANALOG_4 4 ///< Mapped to PC4
#define JOYSTICK_ANALOG_5 5 ///< Mapped to PC5

// Joystick Button selection
#define JOYSTICK_BUTTON_0 5 ///< Mapped to PD5
#define JOYSTICK_BUTTON_1 6 ///< Mapped to PD6
#define JOYSTICK_BUTTON_2 7 ///< Mapped to PD7

/**
 * @struct joystick
 * @brief
 */
typedef struct joystick
{
    unsigned int x;       ///< Value on X axis
    unsigned int y;       ///< Value on Y axis
    unsigned char button; ///< State of the button
} joystick_t;

/**
 * @brief Initialize a joystick by configuring pins as inputs
 * @param x Analog pin X
 * @param y Analog pin Y
 * @param btn Digital pin of the button
 */
void JOYSTICK_init(byte_t x, byte_t y, byte_t btn);

/**
 * @brief Read the values from the joystick
 * @param joy Pointer to the joystick structure to store the result
 * @param x Analog pin X
 * @param y Analog pin Y
 * @param btn Digital pin of the button
 */
void JOYSTICK_read(struct joystick *joy, byte_t x, byte_t y, byte_t btn);

#endif // VEMAR_JOYSTICK

/**
 * @file joystick.h
 * @brief Joystick manipulation
 */
