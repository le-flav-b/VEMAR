#include "src/LDS_YDLIDAR_X2_X2L.h"
#include <ESP32Servo.h>
#include <Wire.h>

#define SERVO_A_PIN 25
#define SERVO_B_PIN 26
#define SERVO_DEG_REEL_MIN 45
#define SERVO_DEG_REEL_MAX 135
#define SERVO_MOUV_DEG_1 1
#define SERVO_MOUV_DEG_2 5

#define LIDAR_EN_PIN 2
#define LIDAR_PWM_PIN 4
#define LIDAR_RX_PIN 16
#define LIDAR_PWM_FREQ 10000
#define LIDAR_PWM_BITS 11
#define LIDAR_PWM_CHANNEL 2

#define I2C_ADDR 0x0C
#define I2C_CMD_SERVO_A_L1 0x10
#define I2C_CMD_SERVO_A_L2 0x11
#define I2C_CMD_SERVO_A_R1 0x12
#define I2C_CMD_SERVO_A_R2 0x13
#define I2C_CMD_SERVO_B_L1 0x14
#define I2C_CMD_SERVO_B_L2 0x15
#define I2C_CMD_SERVO_B_R1 0x16
#define I2C_CMD_SERVO_B_R2 0x17
#define I2C_CMD_LIDAR_ON 0x20
#define I2C_CMD_LIDAR_OFF 0x21

Servo servoA; uint8_t _servo_a_pos = 90;
Servo servoB; uint8_t _servo_b_pos = 90;

void servo_update_pos(void)
{
	if (_servo_a_pos < SERVO_DEG_REEL_MIN) _servo_a_pos = SERVO_DEG_REEL_MIN;
	if (_servo_a_pos > SERVO_DEG_REEL_MAX) _servo_a_pos = SERVO_DEG_REEL_MAX;
	if (_servo_b_pos < SERVO_DEG_REEL_MIN) _servo_b_pos = SERVO_DEG_REEL_MIN;
	if (_servo_b_pos > SERVO_DEG_REEL_MAX) _servo_b_pos = SERVO_DEG_REEL_MAX;
	servoA.write(_servo_a_pos);
	servoB.write(_servo_b_pos);
}

void onReceive(int len) {
	while (Wire.available()) {
		uint8_t cmd = Wire.read();

		switch (cmd) {
			// --- Servo A ---
			case I2C_CMD_SERVO_A_L1: _servo_a_pos -= SERVO_MOUV_DEG_1; servo_update_pos(); break;
			case I2C_CMD_SERVO_A_L2: _servo_a_pos -= SERVO_MOUV_DEG_2; servo_update_pos(); break;
			case I2C_CMD_SERVO_A_R1: _servo_a_pos += SERVO_MOUV_DEG_1; servo_update_pos(); break;
			case I2C_CMD_SERVO_A_R2: _servo_a_pos += SERVO_MOUV_DEG_2; servo_update_pos(); break;
			// --- Servo B ---
			case I2C_CMD_SERVO_B_L1: _servo_b_pos -= SERVO_MOUV_DEG_1; servo_update_pos(); break;
			case I2C_CMD_SERVO_B_L2: _servo_b_pos -= SERVO_MOUV_DEG_2; servo_update_pos(); break;
			case I2C_CMD_SERVO_B_R1: _servo_b_pos += SERVO_MOUV_DEG_1; servo_update_pos(); break;
			case I2C_CMD_SERVO_B_R2: _servo_b_pos += SERVO_MOUV_DEG_2; servo_update_pos(); break;
			// --- Lidar ---
			case I2C_CMD_LIDAR_ON: digitalWrite(LIDAR_EN_PIN, HIGH); break;
			case I2C_CMD_LIDAR_OFF: digitalWrite(LIDAR_EN_PIN, LOW); break;
			default: break;
		}
	}
}

HardwareSerial LidarSerial(1);
LDS_YDLIDAR_X2_X2L lidar;

int tab_dist_deg_cm[360]; // motor is 0°, CW

void onRequest() {
	Wire.write(tab_dist_deg_cm[180]); // TODO
}

void setup() {
	for (int i = 0; i < 360; i++) tab_dist_deg_cm[i] = 0;
	Serial.begin(115200);

	pinMode(LIDAR_EN_PIN, OUTPUT); digitalWrite(LIDAR_EN_PIN, LOW);
	LidarSerial.begin(115200, SERIAL_8N1, LIDAR_RX_PIN);
	lidar.setScanPointCallback(lidar_scan_point_callback);
	lidar.setPacketCallback(lidar_packet_callback);
	lidar.setSerialWriteCallback(lidar_serial_write_callback);
	lidar.setSerialReadCallback(lidar_serial_read_callback);
	lidar.setMotorPinCallback(lidar_motor_pin_callback);
	lidar.init();
	lidar.start();

	servoA.attach(SERVO_A_PIN, 500, 2500); servoA.write(_servo_a_pos);
	servoB.attach(SERVO_B_PIN, 500, 2500); servoB.write(_servo_b_pos);

	Wire.begin(I2C_ADDR);
	Wire.onReceive(onReceive);
	Wire.onRequest(onRequest);
}

int lidar_serial_read_callback() { return LidarSerial.read(); }

size_t lidar_serial_write_callback(const uint8_t * buffer, size_t length) { return LidarSerial.write(buffer, length); }

void lidar_scan_point_callback(float angle_deg, float distance_mm, float quality, bool scan_completed) {
	tab_dist_deg_cm[(int)angle_deg] = (int)(distance_mm / 10);
}

void lidar_motor_pin_callback(float value, LDS::lds_pin_t lidar_pin) {
	if (value <= (float)LDS::DIR_INPUT)
		pinMode(LIDAR_PWM_PIN, (value == (float)LDS::DIR_INPUT) ? INPUT : OUTPUT);
	else
		digitalWrite(LIDAR_PWM_PIN, (value == (float)LDS::VALUE_HIGH) ? HIGH : LOW);
}

void lidar_packet_callback(uint8_t * packet, uint16_t length, bool scan_completed) { return; }

void loop()
{
	lidar.loop();
	if (Serial.available()) {
		for (int i = 0; i < 360; i++) {
			Serial.print(i);
			Serial.print("°: ");
			Serial.print(tab_dist_deg_cm[i]);
			Serial.print("cm | ");
		}
		Serial.println();
	}
}
