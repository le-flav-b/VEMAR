#include "led.h"
#include "motor.h"


int main()
{
	motor_init();

	while (1)
	{
		motor_left_set(-100);
		motor_right_set(100);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);

		motor_left_set(-255);
		motor_right_set(255);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);

		motor_left_set(100);
		motor_right_set(-100);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);

		motor_left_set(255);
		motor_right_set(-255);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);
	}
}
