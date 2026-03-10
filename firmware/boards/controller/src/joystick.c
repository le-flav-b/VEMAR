#include "joystick.h"

joystick_t JOYSTICK_new(adc_ch_t x, adc_ch_t y, pin_t button)
{
    joystick_t retval = {
        .x = ANALOG_new(x),
        .y = ANALOG_new(y),
        .button = BUTTON_new(button, BUTTON_ONHOLD)};

    return (retval);
}

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
extern inline uint16_t JOYSTICK_x(joystick_t *joystick);
extern inline uint16_t JOYSTICK_y(joystick_t *joystick);
extern inline bool_t JOYSTICK_is_pressed(joystick_t *joystick);
