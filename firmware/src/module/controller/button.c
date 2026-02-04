#include "button.h"
#include "pin.h"

#define BUTTON_MASK_STATE 0x03
#define BUTTON_MASK_TRIGGER 0x1C

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

struct button BUTTON_new(byte_t pin, byte_t trigger)
{
	struct button retval = {
		.pin = pin,
		.flags = trigger};

	PIN_mode(pin, PIN_INPUT); // configure pin as input
	return (retval);
}

// void BUTTON_read(button_t *btn)
// {
// 	// the last two bits of `flags` indicate the state of the button
// 	byte_t state = ((btn->flags << 1) & BUTTON_MASK_STATE) |
// 				   (PIN_LOW == PIN_read(btn->pin));
// 	BIT_write(btn->flags, state, BUTTON_MASK_STATE); // update the state
// 	delay(DEBOUNCE_TIME);							 // debouncing
// }

bool_t BUTTON_ispressed(button_t *btn)
{
	delay(DEBOUNCE_TIME);							 // debouncing
	byte_t state = ((btn->flags << 1) & BUTTON_MASK_STATE) |
				   (PIN_LOW == PIN_read(btn->pin));
	BIT_write(btn->flags, state, BUTTON_MASK_STATE); // update the state

	// byte_t state = (btn.flags & BUTTON_MASK_STATE);

	switch (btn->flags & BUTTON_MASK_TRIGGER)
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
