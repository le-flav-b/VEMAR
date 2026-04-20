#include "led.h"

int main()
{
	led_init();
	led_red_on();

	while (1)
	{
		_delay_ms(1000);
		led_green_toggle(); led_red_toggle();
	}
}
