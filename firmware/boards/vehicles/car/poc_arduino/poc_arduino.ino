#define PIN_M1_EN_CW 7
#define PIN_M1_EN_CCW 8
#define PIN_M1_PWM_CW 5
#define PIN_M1_PWM_CCW 6

#define PIN_M2_EN_CW 14
#define PIN_M2_EN_CCW 15
#define PIN_M2_PWM_CW 9
#define PIN_M2_PWM_CCW 10


void setup() {
  pinMode(PIN_M1_EN_CW, OUTPUT);
  pinMode(PIN_M1_EN_CCW, OUTPUT);
  pinMode(PIN_M1_PWM_CW, OUTPUT);
  pinMode(PIN_M1_PWM_CCW, OUTPUT);

  pinMode(PIN_M2_EN_CW, OUTPUT);
  pinMode(PIN_M2_EN_CCW, OUTPUT);
  pinMode(PIN_M2_PWM_CW, OUTPUT);
  pinMode(PIN_M2_PWM_CCW, OUTPUT);

  off();
}

void off() {
  digitalWrite(PIN_M1_EN_CW, LOW);
  digitalWrite(PIN_M1_PWM_CW, LOW);
  digitalWrite(PIN_M1_EN_CCW, LOW);
  digitalWrite(PIN_M1_PWM_CCW, LOW);
  digitalWrite(PIN_M2_EN_CW, LOW);
  digitalWrite(PIN_M2_PWM_CW, LOW);
  digitalWrite(PIN_M2_EN_CCW, LOW);
  digitalWrite(PIN_M2_PWM_CCW, LOW);
  delay(2);
}


void on() {
  digitalWrite(PIN_M1_EN_CW, HIGH);
  analogWrite(PIN_M1_PWM_CW, 255);
  digitalWrite(PIN_M2_EN_CW, HIGH);
  analogWrite(PIN_M2_PWM_CW, 255);
}

void loop() {
  digitalWrite(PIN_M1_EN_CW, HIGH);
  analogWrite(PIN_M1_PWM_CW, 100);
  digitalWrite(PIN_M2_EN_CW, HIGH);
  analogWrite(PIN_M2_PWM_CW, 100);
  delay(5000);

  off();
  delay(2000);

  digitalWrite(PIN_M1_EN_CW, HIGH);
  analogWrite(PIN_M1_PWM_CW, 255);
  digitalWrite(PIN_M2_EN_CW, HIGH);
  analogWrite(PIN_M2_PWM_CW, 255);
  delay(5000);

  off();
  delay(2000);

  digitalWrite(PIN_M1_EN_CCW, HIGH);
  analogWrite(PIN_M1_PWM_CCW, 100);
  digitalWrite(PIN_M2_EN_CCW, HIGH);
  analogWrite(PIN_M2_PWM_CCW, 100);
  delay(5000);

  off();
  delay(2000);

  digitalWrite(PIN_M1_EN_CCW, HIGH);
  analogWrite(PIN_M1_PWM_CCW, 255);
  digitalWrite(PIN_M2_EN_CCW, HIGH);
  analogWrite(PIN_M2_PWM_CCW, 255);
  delay(5000);

  off();
  delay(2000);
}
