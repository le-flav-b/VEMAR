#include "joystick.h"
#include "serial.h"

joystick_t ljoy;
joystick_t rjoy;

typedef struct
{
	unsigned int x;
	unsigned int y;
	bool_t press;
} value_t;

value_t lvalue;
value_t rvalue;

static void display(char n, joystick_t *joy, value_t *old)
{
	value_t new = {
		.x = JOYSTICK_x(joy),
		.y = JOYSTICK_y(joy),
		.press = JOYSTICK_is_pressed(joy)};

	if ((new.x == old->x) && (new.y == old->y) && (new.press == old->press))
	{
		return;
	}
	old->x = new.x;
	old->y = new.y;
	old->press = new.press;

	SERIAL_print(str, "JOY ");
	SERIAL_print(char, n);
	SERIAL_print(str, " (X, Y) = (");
	SERIAL_print(uint, new.x);
	SERIAL_print(str, ", ");
	SERIAL_print(uint, new.y);
	if (new.press)
	{
		SERIAL_println(str, "), PRESSED");
	}
	else
	{
		SERIAL_println(str, "), released");
	}
}

void setup(void)
{
	SERIAL_init();
	rjoy = JOYSTICK_new(ADC_CHANNEL_1, ADC_CHANNEL_0, PIN_PD2);
	ljoy = JOYSTICK_new(ADC_CHANNEL_7, ADC_CHANNEL_6, PIN_PC5);
}

void loop(void)
{
	display('R', &rjoy, &rvalue);
	display('L', &ljoy, &lvalue);
	delay(1000);
}
