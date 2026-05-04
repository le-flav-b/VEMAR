#include "data_save.h"
#include "led.h"

uint32_t idx = 0;
uint8_t sd_valid = 0;
uint8_t sd_save_enabled = 0;

uint8_t sd_check(void) {
    uint8_t res = SD_init(&DDRD, &PORTD, (1 << PD4));
    sd_valid = (res == SD_OK);
    return sd_valid;
}

uint8_t sd_file_exists(const char *name) {
    return (SD_json_open(name) == SD_OK);
}

uint8_t sd_file_create(const char *name) {
    return (SD_json_create(name) == SD_OK);
}

void sd_set_save(uint8_t enabled) {
    sd_save_enabled = enabled;
}

uint8_t sd_prepare(void) {
    if (sd_check() == 0) {
        return 1;
    }
    if (sd_file_exists("SENSORS") == 0) {
        if (sd_file_create("SENSORS") == 0) {
            return 1;
        }
        SD_json_close();
    }
    return 0;
}

static uint8_t sd_open(void) {
    if (!sd_save_enabled) {
        return 0;
    }
    if (!sd_valid && sd_check() == 0) {
        return 0;
    }
    if (SD_json_open("SENSORS") == SD_OK) {
        return 1;
    }
    /* Retry after re-initializing SPI/SD. */
    if (sd_check() == 0) {
        return 0;
    }
    return (SD_json_open("SENSORS") == SD_OK);
}

uint8_t sd_append_atmosphere(packet_t *packet) {
    if (!sd_open()) return 0;
    led_green_on();
    if (SD_json_append("idx",   UTIL_itoa(idx++, 0)))                             goto fail;
    if (SD_json_append("temp",  UTIL_itoa_decimal(packet->atmosphere.temperature, 0))) goto fail;
    if (SD_json_append("hum",   UTIL_itoa_decimal(packet->atmosphere.humidity, 0)))    goto fail;
    if (SD_json_append("press", UTIL_itoa_decimal(packet->atmosphere.pressure, 0)))    goto fail;
    if (SD_json_append("pm25",  UTIL_itoa_decimal(packet->atmosphere.pm25, 0)))        goto fail;
    if (SD_json_append("pm10",  UTIL_itoa_decimal(packet->atmosphere.pm10, 0)))        goto fail;
    SD_json_close();
    led_green_off();
    return 0;
fail:
    led_green_off();
    return 1;
}

uint8_t sd_append_gas(packet_t *packet) {
    if (!sd_open()) return 0;
    led_green_on();
    if (SD_json_append("idx", UTIL_itoa(idx++, 0)))           goto fail;
    if (SD_json_append("co2", UTIL_itoa(packet->gas.co2, 0))) goto fail;
    if (SD_json_append("co",  UTIL_itoa(packet->gas.co, 0)))  goto fail;
    if (SD_json_append("nh3", UTIL_itoa(packet->gas.nh3, 0))) goto fail;
    if (SD_json_append("no2", UTIL_itoa(packet->gas.no2, 0))) goto fail;
    if (SD_json_append("o2",  UTIL_itoa(packet->gas.o2, 0)))  goto fail;
    SD_json_close();
    led_green_off();
    return 0;
fail:
    led_green_off();
    return 1;
}

uint8_t sd_append_lidar(packet_t *packet) {
    if (!sd_open()) return 0;
    led_green_on();
    if (SD_json_append("idx", UTIL_itoa(idx++, 0)))           goto fail;
    if (SD_json_append("up",  UTIL_itoa_decimal2(packet->distance.up, 0)))  goto fail;
    if (SD_json_append("down",  UTIL_itoa_decimal2(packet->distance.down, 0)))  goto fail;
    if (SD_json_append("left",  UTIL_itoa_decimal2(packet->distance.left, 0)))  goto fail;
    if (SD_json_append("right",  UTIL_itoa_decimal2(packet->distance.right, 0)))  goto fail;
    SD_json_close();
    led_green_off();
    return 0;
fail:
    led_green_off();
    return 1;
}

uint8_t sd_append_radioactivity(packet_t *packet) {
    if (!sd_open()) return 0;
    led_green_on();
    if (SD_json_append("idx",   UTIL_itoa(idx++, 0)))               goto fail;
    if (SD_json_append("total", UTIL_itoa(packet->geiger.total, 0))) goto fail;
    if (SD_json_append("delta", UTIL_itoa(packet->geiger.delta, 0))) goto fail;
    if (SD_json_append("cpm",   UTIL_itoa(packet->geiger.cpm, 0)))   goto fail;
    SD_json_close();
    led_green_off();
    return 0;
fail:
    led_green_off();
    return 1;
}
