

extern "C" {
  #include "i2c.h"
  #include "atmosphere.h"
}

static int32_t unpack_s32_be(const uint8_t *buf) {
    return ((int32_t)buf[0] << 24) |
           ((int32_t)buf[1] << 16) |
           ((int32_t)buf[2] << 8) |
           (int32_t)buf[3];
}

static uint32_t unpack_u32_be(const uint8_t *buf) {
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8) |
           (uint32_t)buf[3];
}

static uint16_t unpack_u16_le(const uint8_t *buf) {
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

// Read sensor data from slave and print readings
void read_and_print_sensors(void) {
    uint8_t buffer[MAX_LEN] = {0};
    
    // Read packet from slave
    int8_t err = i2c_read_packet(SLAVE_ADDR, buffer);
    if (err != 0) {
        Serial.print("I2C read error: ");
        Serial.println(err);
        return;
    }
    
    // i2c_read_packet strips the [len_hi][len_lo] header.
    // Payload layout is [PM:4][BME:12][sds_valid_frames_lo][sds_rx_bytes_lo].
    uint8_t *pm_data = buffer;
    uint8_t *bme_data = &buffer[PM_PACKET_SIZE];

    uint16_t pm25_raw = unpack_u16_le(&pm_data[0]);
    uint16_t pm10_raw = unpack_u16_le(&pm_data[2]);

    float pm25_ugm3 = (float)pm25_raw / 10.0f;
    float pm10_ugm3 = (float)pm10_raw / 10.0f;

    int32_t temp_centi = unpack_s32_be(&bme_data[0]);
    uint32_t pressure_q24_8 = unpack_u32_be(&bme_data[4]);
    uint32_t humidity_q22_10 = unpack_u32_be(&bme_data[8]);

    float temp_c = (float)temp_centi / 100.0f;
    float pressure_pa = (float)pressure_q24_8 / 256.0f;
    float humidity_rh = (float)humidity_q22_10 / 1024.0f;

    // Print readings
    Serial.println("=== Atmosphere Readings ===");
    Serial.print("PM2.5: ");
    Serial.print(pm25_ugm3, 1);
    Serial.println(" ug/m3");
    Serial.print("PM10:  ");
    Serial.print(pm10_ugm3, 1);
    Serial.println(" ug/m3");
    Serial.print("Temp:  ");
    Serial.print(temp_c, 2);
    Serial.println(" C");
    Serial.print("Press: ");
    Serial.print(pressure_pa, 2);
    Serial.println(" Pa");
    Serial.print("Hum:   ");
    Serial.print(humidity_rh, 2);
    Serial.println(" %RH");
    uint8_t sds_frames = buffer[PM_PACKET_SIZE + BME_PACKET_SIZE];
    uint8_t sds_rx     = buffer[PM_PACKET_SIZE + BME_PACKET_SIZE + 1];
    Serial.print("SDS frames: ");
    Serial.print(sds_frames);
    Serial.print("  RX bytes: ");
    Serial.println(sds_rx);
    Serial.println();
}

int main(void) {
    // Initialize Serial communication
    Serial.begin(9600);
    
    Serial.println("Atmosphere Master Starting...");
    
    // Initialize I2C master
    i2c_init();
    _delay_ms(100);
    
    Serial.println("Waiting for slave data...");
    _delay_ms(500);
    
    // Main loop: read sensors every second
    while (1) {
        read_and_print_sensors();
        _delay_ms(1000);
    }
    
    return 0;
}

