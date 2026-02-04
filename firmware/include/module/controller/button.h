#ifndef VEMAR_BUTTON_H
#define VEMAR_BUTTON_H

#include "common.h"

#define BUTTON_ONPRESS 0x04
#define BUTTON_ONRELEASE 0x08
#define BUTTON_ONHOLD 0x10

/**
 * @brief Button
 */
typedef struct button
{
	byte_t pin;
	byte_t flags;
} button_t;

/**
 * @brief Instanciate a new button
 * @param pin
 * @param trigger
 * @see
 */
button_t BUTTON_new(byte_t pin, byte_t trigger);

/**
 * @brief
 * @param btn
 * @return
 */
bool_t BUTTON_ispressed(button_t *btn);

/**
 * @brief Read the state of the button
 * @param btn Pointer to the button structure
 */
// void BUTTON_read(button_t *btn);

#endif // VEMAR_BUTTON_H
