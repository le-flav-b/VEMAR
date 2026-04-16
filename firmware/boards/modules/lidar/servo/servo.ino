#include <Servo.h>

Servo myservo;

void setup() {
  myservo.attach(9);
}6

void loop() {
  myservo.write(100);
  delay(1000);
  myservo.write(140);
  delay(250);
  myservo.write(60);
  delay(250);
}
