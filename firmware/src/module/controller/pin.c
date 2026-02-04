#include "pin.h"

#define REG_PIN(pin) (_SFR_IO8(0x03 + ((pin & 0xF0) >> 4)))
#define REG_DDR(pin) (_SFR_IO8(0x04 + ((pin & 0xF0) >> 4)))
#define REG_PORT(pin) (_SFR_IO8(0x05 + ((pin & 0xF0) >> 4)))

#define REG_SHIFT(pin, value) (value << (pin & 0x0F))

byte_t PIN_read(byte_t pin)
{
	return (BIT_read(REG_PIN(pin), REG_SHIFT(pin, 1)));
}

void PIN_write(byte_t pin, byte_t state)
{
	BIT_write(REG_PORT(pin), REG_SHIFT(pin, state), REG_SHIFT(pin, 1));
}

void PIN_toggle(byte_t pin)
{
	BIT_write(REG_PIN(pin), REG_SHIFT(pin, 1), REG_SHIFT(pin, 1));
}

void PIN_mode(byte_t pin, byte_t mode)
{
	BIT_write(REG_DDR(pin), REG_SHIFT(pin, mode), REG_SHIFT(pin, 1));
}

