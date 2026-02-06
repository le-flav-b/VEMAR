#ifndef VEMAR_GPIO_H
#define VEMAR_GPIO_H

#include "common.h"
#include "adc.h"

/**
 * @brief Pins selections
 *
 * - Port B is an 8-bit bi-directional I/O port
 * - Port C is a 7-bit bi-directional I/O port
 * - Port D is an 8-bit bi-directional I/O port
 */
typedef enum vemar_enum_pin
{
    PIN_PB0 = 0x00, ///< Port B 0
    PIN_PB1 = 0x01, ///< Port B 1
    PIN_PB2 = 0x02, ///< Port B 2
    PIN_PB3 = 0x03, ///< Port B 3
    PIN_PB4 = 0x04, ///< Port B 4
    PIN_PB5 = 0x05, ///< Port B 5
    PIN_PB6 = 0x06, ///< Port B 6
    PIN_PB7 = 0x07, ///< Port B 7
    PIN_PC0 = 0x30, ///< Port C 0
    PIN_PC1 = 0x31, ///< Port C 1
    PIN_PC2 = 0x32, ///< Port C 2
    PIN_PC3 = 0x33, ///< Port C 3
    PIN_PC4 = 0x34, ///< Port C 4
    PIN_PC5 = 0x35, ///< Port C 5
    PIN_PC6 = 0x36, ///< Port C 6
    PIN_PD0 = 0x60, ///< Port C 0
    PIN_PD1 = 0x61, ///< Port D 1
    PIN_PD2 = 0x62, ///< Port D 2
    PIN_PD3 = 0x63, ///< Port D 3
    PIN_PD4 = 0x64, ///< Port D 4
    PIN_PD5 = 0x65, ///< Port D 5
    PIN_PD6 = 0x66, ///< Port D 6
    PIN_PD7 = 0x67  ///< Port D 7
} pin_t;

/**
 * @brief Pin mode selections
 */
typedef enum vemar_enum_pin_mode
{
    PIN_INPUT = 0, ///< Configure pin as `INPUT`
    PIN_OUTPUT = 1 ///< Configure pin as `OUTPUT`
} pin_mode_t;

/**
 * @brief Pin state selections
 */
typedef enum vemar_enum_pin_state
{
    PIN_LOW = 0, ///< Set pin value to `LOW`
    PIN_HIGH = 1 ///< Set pin value to `HIGH`
} pin_state_t;

/**
 * @brief Button mode selections
 */
typedef enum vemar_enum_button_mode
{
    BUTTON_ONPRESS = 0x04,   ///< Button active on press
    BUTTON_ONRELEASE = 0x08, ///< Button active on release
    BUTTON_ONHOLD = 0x10     ///< Button active on hold
} button_mode_t;

/**
 * @brief Structure for LED
 */
typedef struct vemar_struct_led
{
    pin_t pin; ///< Pin of the LED
} led_t;

/**
 * @brief Structure for Button
 */
typedef struct vemar_struct_button
{
    pin_t pin;    ///< Pin of the Button
    byte_t flags; ///< Button flags
} button_t;

/**
 * @brief Structure for Analog
 */
typedef struct vemar_struct_analog
{
    pin_t pin; ///< Pin for the Analog
} analog_t;

//------------------------------------------------------------------------------
// PIN configuration functions
//------------------------------------------------------------------------------

/**
 * @brief Read the state from the specified pin
 * @param pin Pin to read
 * @return `PIN_LOW` or `PIN_HIGH`
 */
pin_state_t PIN_read(pin_t pin);

/**
 * @brief Write the state to the specified pin
 * @param pin Pin to write
 * @param state State of the pin
 */
void PIN_write(pin_t pin, pin_state_t state);

/**
 * @brief Toggle the state of the specified pin
 * @param pin Pin to toggle
 */
void PIN_toggle(pin_t pin);

/**
 * @brief Configure the specified pin as either input or output
 * @param pin Pin to configure
 * @param mode Mode fo the pin:
 */
void PIN_mode(pin_t pin, pin_mode_t mode);

//------------------------------------------------------------------------------
// LED
//------------------------------------------------------------------------------

/**
 * @brief Instanciate a new LED
 * @param pin Pin of the LED
 */
led_t LED_new(pin_t pin);

/**
 * @brief Check Whether an LED is turn on
 * @param led LED to check
 * @return Non-zero value if the LED is on, otherwise `0`
 */
inline bool_t LED_is_on(led_t led)
{
    return (PIN_HIGH == PIN_read(led.pin));
}

/**
 * @brief Turn an LED on
 * @param led LED to turn on
 */
inline void LED_on(led_t led)
{
    PIN_write(led.pin, PIN_HIGH);
}

/**
 * @brief Turn an LED off
 * @param led LED to turn off
 */
inline void LED_off(led_t led)
{
    PIN_write(led.pin, PIN_LOW);
}

/**
 * @brief Toggle an LED
 * @param led LED to toggle
 */
inline void LED_toggle(led_t led)
{
    PIN_toggle(led.pin);
}

//------------------------------------------------------------------------------
// Button
//------------------------------------------------------------------------------

/**
 * @brief Instanciate a new button
 * @param pin Pin of the button
 * @param mode Mode of the button
 *
 * @see button_mode_t
 */
button_t BUTTON_new(pin_t pin, button_mode_t mode);

/**
 * @brief Check whether the button is active
 * @param btn Button to check
 * @return Non-zero value if the button is active, otherwise `0`
 */
bool_t BUTTON_is_active(button_t *btn);

//------------------------------------------------------------------------------
// Analog
//------------------------------------------------------------------------------

analog_t ANALOG_new(pin_t pin);

unsigned int ANALOG_read(analog_t analog);

#endif // VEMAR_GPIO_H

/**
 * @file gpio.h
 * @brief General-Purpose Input/Output
 * @author Christian Hugon
 * @version 0.0.1
 */
