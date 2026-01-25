void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(1000);

  Serial.println("BNO055 UART OK");

  setConfigMode();
  setNDOFMode();
}

void loop() {
  requestEuler();
  delay(50);

  if (Serial1.available() >= 9) {
    uint8_t hdr = Serial1.read();   // 0xBB
    uint8_t len = Serial1.read();   // 6

    int16_t heading = Serial1.read() | (Serial1.read() << 8);
    int16_t roll    = Serial1.read() | (Serial1.read() << 8);
    int16_t pitch   = Serial1.read() | (Serial1.read() << 8);

    Serial.print("Yaw: ");
    Serial.print(heading / 16.0);
    Serial.print("  Roll: ");
    Serial.print(roll / 16.0);
    Serial.print("  Pitch: ");
    Serial.println(pitch / 16.0);
  }

  delay(100);
}

void setConfigMode() {
  uint8_t cmd[] = {0xAA, 0x00, 0x3D, 0x01, 0x00};
  Serial1.write(cmd, sizeof(cmd));
  delay(30);
}

void setNDOFMode() {
  uint8_t cmd[] = {0xAA, 0x00, 0x3D, 0x01, 0x0C};
  Serial1.write(cmd, sizeof(cmd));
  delay(30);
}

void requestEuler() {
  uint8_t cmd[] = {0xAA, 0x01, 0x1A, 0x06};
  Serial1.write(cmd, sizeof(cmd));
}
