#include <i2c.h>

#include "geiger.h"

#define READ_INTERVAL_MS 1000
#define CPM_MULTIPLIER (60000 / READ_INTERVAL_MS)

static uint32_t g_count;
static uint32_t g_last_count;
static uint32_t g_delta;

static uint32_t unpack_u32_be(const uint8_t *buf)
{
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8) |
           (uint32_t)buf[3];
}

bool_t GEIGER_do_read(void)
{
    uint8_t buf[4] = {0};
    int8_t err = i2c_read_packet(GEIGER_ADDR, buf);
    if (0 != err)
    {
        return (FALSE);
    }
    g_count = unpack_u32_be(buf);
    g_delta = g_count - g_last_count;
    g_last_count = g_count;
    return (TRUE);
}

uint32_t GEIGER_total(void)
{
    return (g_count);
}

uint32_t GEIGER_delta(void)
{
    return (g_delta);
}

uint32_t GEIGER_cpm(void)
{
    return (g_delta * CPM_MULTIPLIER);
}
