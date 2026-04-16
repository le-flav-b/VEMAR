#include "main.h"

/*
#define PIN_M1_EN_CW 7
#define PIN_M1_EN_CCW 8
#define PIN_M1_PWM_CW 5
#define PIN_M1_PWM_CCW 6

void setup() {
  pinMode(PIN_M1_EN_CW, OUTPUT);
  pinMode(PIN_M1_EN_CCW, OUTPUT);
  pinMode(PIN_M1_PWM_CW, OUTPUT);
  pinMode(PIN_M1_PWM_CCW, OUTPUT);

  digitalWrite(PIN_M1_EN_CCW, LOW);
  digitalWrite(PIN_M1_PWM_CCW, LOW);

  digitalWrite(PIN_M1_EN_CW, HIGH);
  analogWrite(PIN_M1_PWM_CW, 128);
}

void loop() {}
*/


int main()
{
	init();
	startup_routine();
	init_clear_interrupt();
	sei();
	while (1) { input_set_time(); }
}
