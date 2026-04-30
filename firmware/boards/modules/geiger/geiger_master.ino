extern "C" {
    #include "i2c.h"
}

#define GEIGER_ADDR        0x28
#define SLAVE_ADDR        0x28
#define READ_INTERVAL_MS   1000
#define CPM_MULTIPLIER     (60000 / READ_INTERVAL_MS)

static uint32_t last_count = 0;

static uint32_t unpack_u32_be(const uint8_t *buf) {
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8)  |
           (uint32_t)buf[3];
}

int main(void) {
    Serial.begin(9600);
    i2c_init();
    _delay_ms(100);

    Serial.println("Geiger master started");

    while (1) {
        uint8_t buf[4] = {0};
        int8_t err = i2c_read_packet(GEIGER_ADDR, buf);

        if (err != 0) {
            Serial.print("I2C error: ");
            Serial.println(err);
        } else {
            uint32_t count = unpack_u32_be(buf);
            uint32_t delta = count - last_count;
            last_count     = count;

            Serial.print("total=");
            Serial.print(count);
            Serial.print("  delta=");
            Serial.print(delta);
            Serial.print("  CPM=");
            Serial.println(delta * CPM_MULTIPLIER);
        }

        _delay_ms(READ_INTERVAL_MS);
    }

    return 0;
}
