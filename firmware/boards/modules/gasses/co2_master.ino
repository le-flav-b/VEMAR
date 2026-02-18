#include <Arduino.h>
#include <Wire.h>

#define CAL_BUTTON_PIN 3  // calibration button on PA3 on ATtiny1614
#define I2C_ADDRESS 0x28  // The I2C address this ATtiny will use

// MH-Z19 Commands
byte readCmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte calCmd[9]  = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
byte response[9];

uint16_t co2Value = 0;

void setup() {
  pinMode(CAL_BUTTON_PIN, INPUT_PULLUP);
  
  // Start UART for Sensor (9600 baud)
  Serial.begin(9600); 
  
  // Start I2C as a Slave/Target
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent); // Function to call when main MCU asks for data

  delay(2000); // Wait for sensor to wake up

  // Check if button is held down at boot for calibration
  if (digitalRead(CAL_BUTTON_PIN) == LOW) {
    Serial.write(calCmd, 9);
    // Optional: Blink an LED fast here to confirm calibration command sent
    delay(5000); 
  }
}

void loop() {
  static unsigned long lastRequest = 0;
  
  // Request data from sensor every 5 seconds
  if (millis() - lastRequest > 5000) {
    Serial.write(readCmd, 9);
    Serial.readBytes(response, 9);
    
    if (response[0] == 0xFF && response[1] == 0x86) {
      // Calculate CO2: (High Byte * 256) + Low Byte
      co2Value = (response[2] << 8) | response[3];
    }
    lastRequest = millis();
  }
}

// This function runs when the Main MCU requests data via I2C
void requestEvent() {
  // Split the 16-bit CO2 value into two bytes to send over I2C
  byte data[2];
  data[0] = (co2Value >> 8) & 0xFF;
  data[1] = co2Value & 0xFF;
  Wire.write(data, 2);
}
