#ifndef VEMAR_GEIGER_H
#define VEMAR_GEIGER_H

#include "typedef.h"

#define GEIGER_ADDR 0x28
// #define SLAVE_ADDR 0x28

bool_t GEIGER_do_read(void);

uint32_t GEIGER_total(void);

uint32_t GEIGER_delta(void);

uint32_t GEIGER_cpm(void);

#endif // VEMAR_GEIGER_H
