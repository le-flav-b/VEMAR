#include "gpio.h"

// access corresponding register: PINx, DDRx, PORTx
#define REG_PIN(pin) (_SFR_IO8(0x03 + ((pin & 0xF0) >> 4)))
#define REG_DDR(pin) (_SFR_IO8(0x04 + ((pin & 0xF0) >> 4)))
#define REG_PORT(pin) (_SFR_IO8(0x05 + ((pin & 0xF0) >> 4)))

// shift to corresponding register bit
#define REG_SHIFT(pin, value) (value << (pin & 0x0F))

// mask for button flags
#define BUTTON_MASK_STATE 0x03
#define BUTTON_MASK_TRIGGER 0x1C

// convert analog pin to adc channel
#define ANALOG_TO_ADC_CHANNEL(pin) \
	((pin) & 0x0F)

/**
 * | Bit 1 | Bit 0 | Description |
 * | ----- | ----- | ----------- |
 * |   0   |   0   | Released    |
 * |   0   |   1   | On Press    |
 * |   1   |   0   | On Release  |
 * |   1   |   1   | On Hold     |
 */
#define BUTTON_STATE_ONPRESS 0x01
#define BUTTON_STATE_ONRELEASE 0x02
#define BUTTON_STATE_ONHOLD 0x03

//------------------------------------------------------------------------------
// Pin
//------------------------------------------------------------------------------

pin_state_t PIN_read(pin_t pin)
{
	return (BIT_read(REG_PIN(pin), REG_SHIFT(pin, 1)));
}

void PIN_write(pin_t pin, pin_state_t state)
{
	BIT_write(REG_PORT(pin), REG_SHIFT(pin, state), REG_SHIFT(pin, 1));
}

void PIN_toggle(pin_t pin)
{
	BIT_write(REG_PIN(pin), REG_SHIFT(pin, 1), REG_SHIFT(pin, 1));
}

void PIN_mode(pin_t pin, pin_mode_t mode)
{
	BIT_write(REG_DDR(pin), REG_SHIFT(pin, mode), REG_SHIFT(pin, 1));
	// enable pull-up resistor / set output high
	BIT_write(REG_PORT(pin), REG_SHIFT(pin, 1), REG_SHIFT(pin, 1));
}

//------------------------------------------------------------------------------
// LED
//------------------------------------------------------------------------------

led_t LED_new(pin_t pin)
{
    led_t retval = {.pin = pin};

    PIN_mode(pin, PIN_OUTPUT); // configure pin as output
    return (retval);
}

//------------------------------------------------------------------------------
// Button
//------------------------------------------------------------------------------

button_t BUTTON_new(pin_t pin, button_mode_t trigger)
{
    button_t retval = {
        .pin = pin,
        .flags = trigger};

    PIN_mode(pin, PIN_INPUT); // configure pin as input
    return (retval);
}

bool_t BUTTON_is_active(button_t *button)
{
    byte_t state = ((button->flags << 1) & BUTTON_MASK_STATE) |
                   (PIN_LOW == PIN_read(button->pin));

    BIT_write(button->flags, state, BUTTON_MASK_STATE); // update the state
    delay(DEBOUNCE_TIME);                            // debouncing

    switch (button->flags & BUTTON_MASK_TRIGGER)
    {
    case BUTTON_ONHOLD:
        return (BUTTON_STATE_ONHOLD == state);
    case BUTTON_ONPRESS:
        return (BUTTON_STATE_ONPRESS == state);
    case BUTTON_ONRELEASE:
        return (BUTTON_STATE_ONRELEASE == state);
    default:
        break;
    }
    return (0);
}

//------------------------------------------------------------------------------
// Analog
//------------------------------------------------------------------------------

analog_t ANALOG_new(pin_t pin)
{
	analog_t retval = {.pin = pin};

	ADC_enable_channel(ANALOG_TO_ADC_CHANNEL(pin));
	return (retval);
}

unsigned int ANALOG_read(analog_t analog)
{
	return (ADC_read(ANALOG_TO_ADC_CHANNEL(analog.pin)));
}

//------------------------------------------------------------------------------
// Inline functions
//------------------------------------------------------------------------------
extern inline bool_t LED_is_on(led_t);
extern inline void LED_on(led_t);
extern inline void LED_off(led_t);
extern inline void LED_toggle(led_t);
