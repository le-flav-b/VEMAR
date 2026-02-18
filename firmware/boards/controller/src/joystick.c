#include "joystick.h"
// #include "adc.h"

joystick_t JOYSTICK_new(pin_t x, pin_t y, pin_t button)
{
    joystick_t retval = {
        .x = ANALOG_new(x),
        .y = ANALOG_new(y),
        .button = BUTTON_new(button, BUTTON_ONHOLD)};

    return (retval);
}

extern inline unsigned int JOYSTICK_x(joystick_t *joystick);
extern inline unsigned int JOYSTICK_y(joystick_t *joystick);
extern inline bool_t JOYSTICK_is_pressed(joystick_t *joystick);

/// Return ADC channel corresponding to the pin
// #define PIN_TO_ADC(pin) (pin & 0x0F)

// void JOYSTICK_init(byte_t pin_x, byte_t pin_y, byte_t pin_button)
// {
//     PIN_mode(pin_x, PIN_INPUT);
//     PIN_mode(pin_y, PIN_INPUT);
//     PIN_mode(pin_button, PIN_INPUT);

//     // ADC_enable_channel(PIN_TO_ADC(pin_x));
//     // ADC_enable_channel(PIN_TO_ADC(pin_y));
// }

// void JOYSTICK_read(struct joystick *joy, byte_t pin_x, byte_t pin_y, byte_t pin_button)
// {
//     joy->x = ADC_read(PIN_TO_ADC(pin_x));
//     joy->y = ADC_read(PIN_TO_ADC(pin_y));
//     (joy->button).pressed = (PIN_LOW == PIN_read(pin_button));
// }
