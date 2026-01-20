#include <Wire.h>

void setup() {
  Wire.begin();  // Join as master
  Serial.begin(9600);
}

void loop() {
  Wire.requestFrom(0x42, 1);  // Request 1 byte from slave address 0x42
  
  if (Wire.available()) {
    uint8_t received = Wire.read();
    Serial.print("Received: ");
    Serial.println(received);
  }
  
  delay(1000);
}