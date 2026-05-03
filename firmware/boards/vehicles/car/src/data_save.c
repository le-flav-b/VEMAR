#include "data_save.h"
#include "led.h"

uint32_t idx = 0;
uint8_t sd_valid = 0;
uint8_t file_valid = 0;
uint8_t sd_save_enabled = 0;

// Check if SD card is present by trying to initialize it.  Returns 1 if present, 0 if not.
uint8_t sd_check(void) {    
    uint8_t res = SD_init(&DDRD, &PORTD, (1 << PD4));
    sd_valid = (res == SD_OK);
    return sd_valid;
}

// Check if file exists by trying to open it.  Returns 1 if exists, 0 if not.
uint8_t sd_file_exists(const char *name) {
    uint8_t res = SD_json_open(name);
    file_valid = (res == SD_OK);
    return file_valid;
}

// Create a new file with the given name, containing an empty JSON array "[]\n".  Returns 1 on success, 0 on failure.
uint8_t sd_file_create(const char *name) {
    uint8_t res = SD_json_create(name);
    file_valid = (res == SD_OK);
    return file_valid;
}

void sd_set_save(uint8_t enabled) {
    sd_save_enabled = enabled;
}

uint8_t sd_prepare(void) {
    if (sd_check() == 0) {
        return 1; // SD card not present
    }
    if (sd_file_exists("SENSORS") == 0) {
        if (sd_file_create("SENSORS") == 0) {
            return 1; // Failed to create file
        }
    }
    return 0; // SD card and file are ready
}

// Append atmosphere data to json
uint8_t sd_append_atmosphere(packet_t *packet) {
    if (!sd_save_enabled || !sd_valid || !file_valid) {
        return 0; // SD card or file not valid, cannot append
    }
    char *str;
    uint8_t res = 0; // need to review this error approach
    led_green_on();
    res += SD_json_append("idx", UTIL_itoa(idx++, 0));
    str = UTIL_itoa_decimal(packet->atmosphere.temperature, 0);
    res += SD_json_append("temp", str);
    str = UTIL_itoa_decimal(packet->atmosphere.humidity, 0);
    res += SD_json_append("hum", str);
    str = UTIL_itoa_decimal(packet->atmosphere.pressure, 0);
    res += SD_json_append("press", str);
    str = UTIL_itoa_decimal(packet->atmosphere.pm25, 0);
    res += SD_json_append("pm25", str);
    str = UTIL_itoa_decimal(packet->atmosphere.pm10, 0);
    res += SD_json_append("pm10", str);
    led_green_off();
    return res;
}

// Append gas data to json
uint8_t sd_append_gas(packet_t *packet) {
    if (!sd_save_enabled || !sd_valid || !file_valid) {
        return 0; // SD card or file not valid, cannot append
    }
    char *str;
    uint8_t res = 0; // need to review this error approach
    led_green_on();
    res += SD_json_append("idx", UTIL_itoa(idx++, 0));
    str = UTIL_itoa(packet->gas.co2, 0);
    res += SD_json_append("co2", str);
    str = UTIL_itoa(packet->gas.co, 0);
    res += SD_json_append("co", str);
    str = UTIL_itoa(packet->gas.nh3, 0);
    res += SD_json_append("nh3", str);
    str = UTIL_itoa(packet->gas.no2, 0);
    res += SD_json_append("no2", str);
    str = UTIL_itoa(packet->gas.o2, 0);
    res += SD_json_append("o2", str);
    led_green_off();
    return res;
}

// Append radioactivity data to json
uint8_t sd_append_radioactivity(packet_t *packet) {
    if (!sd_save_enabled || !sd_valid || !file_valid) {
        return 0; // SD card or file not valid, cannot append
    }
    char *str;
    uint8_t res = 0; // need to review this error approach
    led_green_on();
    res += SD_json_append("idx", UTIL_itoa(idx++, 0));
    str = UTIL_itoa(packet->geiger.total, 0);
    res += SD_json_append("total", str);
    str = UTIL_itoa(packet->geiger.delta, 0);
    res += SD_json_append("delta", str);
    str = UTIL_itoa(packet->geiger.cpm, 0);
    res += SD_json_append("cpm", str);
    led_green_off();
    return res;
}
