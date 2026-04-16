#include "atmosphere.h"

extern "C" {
  #include "i2c.h"
}

#define MAX_LEN 64
#define PM_PACKET_SIZE 4

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
    
    // i2c_read_packet already stripped the [len_hi][len_lo] header;
    // buffer[0..3] is the raw PM payload directly.
    // Extract PM data (4 bytes, little-endian from SDS011)
    uint8_t *pm_data = buffer;
    uint16_t pm25 = ((uint16_t)pm_data[1] << 8) | pm_data[0];
    uint16_t pm10 = ((uint16_t)pm_data[3] << 8) | pm_data[2];

    // Print readings
    Serial.println("=== Atmosphere Readings ===");
    Serial.print("PM2.5: ");
    Serial.print(pm25);
    Serial.println(" ug/m3");
    Serial.print("PM10:  ");
    Serial.print(pm10);
    Serial.println(" ug/m3");
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

