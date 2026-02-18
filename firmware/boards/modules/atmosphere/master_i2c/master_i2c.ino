#include <Wire.h>

#define I2C_ADDRESS 0x42  // I2C slave address

volatile uint8_t dataToSend = 42;

void setup() {
  Wire.begin(I2C_ADDRESS);  // Join I2C bus as slave
  Wire.onRequest(requestEvent);  // Register callback for master requests
}

void loop() {
  // You could change dataToSend here if needed
  delay(100);
}

// This function is called when the I2C master requests data
void requestEvent() {
  Wire.write(dataToSend);  // Send the value 42
}