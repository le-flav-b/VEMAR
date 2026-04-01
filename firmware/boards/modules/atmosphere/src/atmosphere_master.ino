#include "atmosphere.h"

extern "C" {
  #include "i2c.h"
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
    
    // Parse packet: [len_hi][len_lo][PM data (4 bytes)][compensated BME data (12 bytes)]
    uint16_t packet_len = ((uint16_t)buffer[0] << 8) | buffer[1];
    
    if (packet_len != (PM_PACKET_SIZE + BME_PACKET_SIZE)) {
        Serial.print("Invalid packet length: ");
        Serial.println(packet_len);
        return;
    }
    
    // Extract PM data (4 bytes)
    uint8_t *pm_data = &buffer[2];
    uint16_t pm25 = ((uint16_t)pm_data[0] << 8) | pm_data[1];
    uint16_t pm10 = ((uint16_t)pm_data[2] << 8) | pm_data[3];
    
    // Extract compensated values from BME data (4 bytes each: temperature, pressure, humidity)
    uint8_t *bme_data = &buffer[2 + PM_PACKET_SIZE];
    
    int32_t temp_comp = ((int32_t)bme_data[0] << 24) | ((uint32_t)bme_data[1] << 16) | 
                        ((uint32_t)bme_data[2] << 8) | bme_data[3];
    uint32_t pres_comp = ((uint32_t)bme_data[4] << 24) | ((uint32_t)bme_data[5] << 16) | 
                         ((uint32_t)bme_data[6] << 8) | bme_data[7];
    uint32_t hum_comp = ((uint32_t)bme_data[8] << 24) | ((uint32_t)bme_data[9] << 16) | 
                        ((uint32_t)bme_data[10] << 8) | bme_data[11];
    
    // Convert to human-readable units
    // Temperature: raw value / 100 = degC
    int16_t temp_int = temp_comp / 100;
    int16_t temp_frac = temp_comp % 100;
    
    // Pressure: raw value / 256 = Pa, convert to hPa
    uint32_t pres_pa = pres_comp / 256;
    uint16_t pres_hpa = pres_pa / 100;
    
    // Humidity: raw value / 1024 = %RH
    uint16_t hum_int = hum_comp / 1024;
    uint16_t hum_frac = (hum_comp % 1024) / 10;
    
    // Print all readings
    Serial.println("=== Atmosphere Readings ===");
    Serial.print("PM2.5: ");
    Serial.print(pm25);
    Serial.println(" µg/m³");
    Serial.print("PM10:  ");
    Serial.print(pm10);
    Serial.println(" µg/m³");
    Serial.print("Temp:  ");
    Serial.print(temp_int);
    Serial.print(".");
    Serial.print(temp_frac);
    Serial.println(" °C");
    Serial.print("Pres:  ");
    Serial.print(pres_hpa);
    Serial.println(" hPa");
    Serial.print("Hum:   ");
    Serial.print(hum_int);
    Serial.print(".");
    Serial.print(hum_frac);
    Serial.println(" %");
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

