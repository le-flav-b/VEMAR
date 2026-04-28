

extern "C" {
  #include "i2c.h"
  #include "atmosphere.h"
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
    uint16_t valid_frames = ((uint16_t)buffer[5] << 8) | buffer[4];
    uint16_t checksum_fail = ((uint16_t)buffer[7] << 8) | buffer[6];
    uint16_t uart_errors = ((uint16_t)buffer[9] << 8) | buffer[8];
    uint16_t rx_bytes = ((uint16_t)buffer[11] << 8) | buffer[10];
    uint16_t aa_headers = ((uint16_t)buffer[13] << 8) | buffer[12];
    uint16_t c0_headers = ((uint16_t)buffer[15] << 8) | buffer[14];
    uint16_t c5_headers = ((uint16_t)buffer[17] << 8) | buffer[16];
    uint16_t query_sent = ((uint16_t)buffer[19] << 8) | buffer[18];
    uint16_t rx_edges = ((uint16_t)buffer[21] << 8) | buffer[20];
    uint8_t reset_flags = buffer[22];

    // Print readings
    Serial.println("=== Atmosphere Readings ===");
    Serial.print("PM2.5: ");
    Serial.print(pm25);
    Serial.println(" ug/m3");
    Serial.print("PM10:  ");
    Serial.print(pm10);
    Serial.println(" ug/m3");
    Serial.print("Valid frames: ");
    Serial.println(valid_frames);
    Serial.print("Checksum fails: ");
    Serial.println(checksum_fail);
    Serial.print("UART error bytes: ");
    Serial.println(uart_errors);
    Serial.print("RX bytes: ");
    Serial.println(rx_bytes);
    Serial.print("AA headers: ");
    Serial.println(aa_headers);
    Serial.print("C0 headers: ");
    Serial.println(c0_headers);
    Serial.print("C5 headers: ");
    Serial.println(c5_headers);
    Serial.print("Query sent: ");
    Serial.println(query_sent);
    Serial.print("RX edges: ");
    Serial.println(rx_edges);
    Serial.print("Reset flags: 0x");
    Serial.println(reset_flags, HEX);
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

