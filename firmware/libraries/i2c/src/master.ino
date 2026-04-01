// #include "i2c.h"

#define SLAVE_ADDR 0x09
#define MAX_LEN 64

extern "C" {
  #include "i2c.h"
}

static void probeSlave(void) {
  int8_t r = i2c_start(SLAVE_ADDR << 1, false);
  i2c_stop();
  Serial.print("probe(SLA+W) result: ");
  Serial.println(r);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("UNO I2C master boot");
  i2c_init();
  probeSlave();
}

void loop() {
  uint8_t buf[MAX_LEN + 1] = {0};

  int8_t result = i2c_read_packet(SLAVE_ADDR, buf);

  Serial.print("result: ");
  Serial.println(result);

  if (result < 0) {
    Serial.print("Error: ");
    Serial.println(result);
    delay(1000);
    return;
  }

  buf[MAX_LEN] = '\0';
  Serial.print("Received: ");
  Serial.println((char *)buf);

  delay(2000);
}