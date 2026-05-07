#include <Wire.h>

#define SLAVE_ADDR 0x0C

// Commands
#define I2C_CMD_SERVO_A_L1 0x10 // q
#define I2C_CMD_SERVO_A_L2 0x11 // w
#define I2C_CMD_SERVO_A_R1 0x12 // e
#define I2C_CMD_SERVO_A_R2 0x13 // r

#define I2C_CMD_SERVO_B_L1 0x14 // t
#define I2C_CMD_SERVO_B_L2 0x15 // y
#define I2C_CMD_SERVO_B_R1 0x16 // u
#define I2C_CMD_SERVO_B_R2 0x17 // i

#define I2C_CMD_LIDAR_ON  0x20 // o
#define I2C_CMD_LIDAR_OFF 0x21 // p

// read is 'a'

void sendCommand(uint8_t cmd) {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(cmd);
  Wire.endTransmission();
}

void readLidar() {
  uint8_t buf[8];
  Wire.requestFrom(SLAVE_ADDR, 8);

  int i = 0;
  while (Wire.available() && i < 8) buf[i++] = Wire.read();

  if (i != 8) { Serial.println("Read error"); return; }

  uint16_t d0   = (buf[0] << 8) | buf[1];
  uint16_t d90  = (buf[2] << 8) | buf[3];
  uint16_t d180 = (buf[4] << 8) | buf[5];
  uint16_t d270 = (buf[6] << 8) | buf[7];

  Serial.print("0°   = ");
  Serial.print(d0 / 100.0, 2);
  Serial.println(" m");

  Serial.print("90°  = ");
  Serial.print(d90 / 100.0, 2);
  Serial.println(" m");

  Serial.print("180° = ");
  Serial.print(d180 / 100.0, 2);
  Serial.println(" m");

  Serial.print("270° = ");
  Serial.print(d270 / 100.0, 2);
  Serial.println(" m");

  Serial.println();
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  if (!Serial.available()) return;
  switch (Serial.read()) {
	case 'q': sendCommand(I2C_CMD_SERVO_A_L1); break;
    case 'w': sendCommand(I2C_CMD_SERVO_A_L2); break;
    case 'e': sendCommand(I2C_CMD_SERVO_A_R1); break;
    case 'r': sendCommand(I2C_CMD_SERVO_A_R2); break;
    case 't': sendCommand(I2C_CMD_SERVO_B_L1); break;
    case 'y': sendCommand(I2C_CMD_SERVO_B_L2); break;
    case 'u': sendCommand(I2C_CMD_SERVO_B_R1); break;
    case 'i': sendCommand(I2C_CMD_SERVO_B_R2); break;
    case 'o': sendCommand(I2C_CMD_LIDAR_ON); break;
    case 'p': sendCommand(I2C_CMD_LIDAR_OFF); break;
    case 'a': readLidar(); break;
    default: break;
  }
}
