extern "C" {
  #include "sd.h"
}
#include <stdlib.h>

#define LED_PIN A2

// Blink codes:
//   1 = SD init failed
//   2 = file open/create failed
//   3 = success after CREATE (first run)
//   4 = append/close failed
//   5 = success after OPEN/APPEND (subsequent runs)
//   6 = storage full (stopped logging)

void blink(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
}

void halt(int errorCode) {
  while (true) {
    blink(errorCode);
    delay(1500);
  }
}

int8_t get_random_temp(void) {
  return rand() % 50;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(10, OUTPUT);

  blink(1); delay(1000);

  if (SD_init(&DDRD, &PORTD, (1 << PD4)) != SD_OK) {
    halt(1);
  }

  uint8_t r = SD_json_open("SENSORS");
  uint8_t created = 0;

  if (r == SD_ERR_NOTFOUND) {
    if (SD_json_create("SENSORS") != SD_OK) halt(2);
    created = 1;
  } else if (r != SD_OK) {
    halt(2);
  }

  // Write 3 entries on first run, 7 on subsequent runs —
  // makes the two sessions clearly distinguishable in the file.
  int count = created ? 3 : 7;
  char buf[8];
  for (int i = 0; i < count; i++) {
    itoa(get_random_temp(), buf, 10);
    uint8_t rc = SD_json_append("temp", buf);
    if (rc == SD_ERR_FULL) {
      SD_json_close();  // best-effort — may fail if truly full
      halt(6);
    }
    if (rc != SD_OK) halt(4);
  }

  uint8_t rc = SD_json_close();
  if (rc == SD_ERR_FULL) halt(6);
  if (rc != SD_OK) halt(4);

  // 3 blinks = created new file, 5 blinks = appended to existing
  halt(created ? 3 : 5);
}

void loop() {
}
