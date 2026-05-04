#ifndef DATA_SAVE_H
#define DATA_SAVE_H

#include "module.h"
#include "sd.h"
#include "util.h"

uint8_t sd_check(void);
uint8_t sd_file_exists(const char *name);
uint8_t sd_file_create(const char *name);
uint8_t sd_prepare(void);
void    sd_set_save(uint8_t enabled);
uint8_t sd_append_atmosphere(packet_t *packet);
uint8_t sd_append_gas(packet_t *packet);
uint8_t sd_append_lidar(packet_t *packet);
uint8_t sd_append_radioactivity(packet_t *packet);

#endif