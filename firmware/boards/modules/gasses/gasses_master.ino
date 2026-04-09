#include <Wire.h>

#define GASSES_ADDR     0x0A
#define PACKET_SIZE     14  // 2-byte length header + 12 bytes payload

// Packet offsets
#define IDX_CO2    2
#define IDX_CO     4
#define IDX_NH3    6
#define IDX_NO2    8
#define IDX_O2     10
#define IDX_TEMP   12
#define IDX_STATUS 13

#define CO2_TEMP_OFFSET  44   // subtract from raw TEMP byte to get °C
// STATUS bits
#define STATUS_CO2_VALID      0x01
#define STATUS_CO2_PREHEATING 0x02

void setup() {
    Serial.begin(115200);
    Wire.begin(); // Arduino Uno as I2C master
    Serial.println("Gasses master ready");
}

void loop() {
    uint8_t buf[PACKET_SIZE] = {0};

    Wire.requestFrom(GASSES_ADDR, PACKET_SIZE);

    uint8_t idx = 0;
    while (Wire.available() && idx < PACKET_SIZE) {
        buf[idx++] = Wire.read();
    }

    if (idx < PACKET_SIZE) {
        Serial.print("Read error: only got ");
        Serial.print(idx);
        Serial.println(" bytes");
        delay(1000);
        return;
    }

    uint16_t len = ((uint16_t)buf[0] << 8) | buf[1];
    if (len != 12) {
        Serial.print("Unexpected payload length: ");
        Serial.println(len);
        delay(1000);
        return;
    }

    uint16_t co2_ppm = ((uint16_t)buf[IDX_CO2]  << 8) | buf[IDX_CO2  + 1];
    uint16_t co_raw  = ((uint16_t)buf[IDX_CO]   << 8) | buf[IDX_CO   + 1];
    uint16_t nh3_raw = ((uint16_t)buf[IDX_NH3]  << 8) | buf[IDX_NH3  + 1];
    uint16_t no2_raw = ((uint16_t)buf[IDX_NO2]  << 8) | buf[IDX_NO2  + 1];
    uint16_t o2_raw  = ((uint16_t)buf[IDX_O2]   << 8) | buf[IDX_O2   + 1];
    int8_t   temp_c  = (int8_t)buf[IDX_TEMP] - CO2_TEMP_OFFSET;
    uint8_t  status  = buf[IDX_STATUS];

    bool co2_valid      = status & STATUS_CO2_VALID;
    bool co2_preheating = status & STATUS_CO2_PREHEATING;

    if (co2_preheating) Serial.println("[CO2 sensor preheating - < 60s uptime]");

    Serial.print("CO2: ");   Serial.print(co2_ppm);
    Serial.println(co2_valid ? " ppm (CRC ok)" : " ppm (CRC pending)");
    Serial.print("Temp(CO2 sensor): "); Serial.print(temp_c); Serial.println(" C");
    Serial.print("CO:  ");   Serial.print(co_raw);  Serial.println(" (raw ADC)");
    Serial.print("NH3: ");   Serial.print(nh3_raw); Serial.println(" (raw ADC)");
    Serial.print("NO2: ");   Serial.print(no2_raw); Serial.println(" (raw ADC)");
    Serial.print("O2:  ");   Serial.print(o2_raw);  Serial.println(" (raw ADC)");
    Serial.println("---");

    delay(2000);
}
