#include "main.h"
#include "uart.h"
#include "uart1.h"

void print_hex(uint8_t byte)
{
	char hex[16] = "0123456789ABCDEF";
	uart_printstr("0x");
	uart_tx(hex[byte / 16]);
	uart_tx(hex[byte % 16]);
	uart_tx(' ');
}

int main(void) {

	char line[20];

	uart_init();
	uart_init_1();

	uart_printstr("BNO055 diagnostic\n\n");

    while (1) {
		_delay_ms(10000);
		uart_printstr("Asking chip ID...\n");
		while (UCSR1A & (1 << RXC1)) {
    		(void)UDR1;
		}
		uart_tx_1(0xAA); uart_tx_1(0x01); uart_tx_1(0x00); uart_tx_1(0x01); // {0xAA, 0x01, 0x00, 0x01}
		uart_printstr("ID : ");
		uint8_t answer[3];
		answer[0] = uart_rx_1(); answer[1] = uart_rx_1(); answer[2] = uart_rx_1();
		print_hex(answer[0]); print_hex(answer[1]); print_hex(answer[2]);
		uart_printstr("\n\n");
    }
}




/*
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
*/
