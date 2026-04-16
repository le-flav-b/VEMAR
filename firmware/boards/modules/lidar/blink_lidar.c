#include "test.h"

int main(void) {
	SET_OUTPUT(DDRD, PD6); // green
	SET_OUTPUT(DDRD, PD7); // red
	SET_OUTPUT(DDRB, PB3); // lidar EN temp

    while (1) {
		SET_OFF(PORTD, PD7);
		SET_ON(PORTD, PD6);
		SET_ON(PORTB, PB3);
        _delay_ms(15000);

		SET_ON(PORTD, PD7);
		SET_OFF(PORTD, PD6);
		SET_OFF(PORTB, PB3);
        _delay_ms(5000);
    }
}
