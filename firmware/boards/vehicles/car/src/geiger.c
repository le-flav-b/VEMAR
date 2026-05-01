#include "module.h"

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

bool_t GEIGER_fill_packet(uint8_t addr, packet_t *packet)
{
    uint8_t buf[4] = {0};

    if (0 != i2c_read_packet(addr, buf))
    {
        return (FALSE);
    }
    g_count = unpack_u32_be(buf);
    g_delta = g_count - g_last_count;
    g_last_count = g_count;

    packet->geiger.id = PACKET_ID_GMC;
    packet->geiger.total = g_count;
    packet->geiger.delta = g_delta;
    packet->geiger.cpm = g_delta * CPM_MULTIPLIER;

    VEMAR_DEBUG(str, "==Geiger Readings==\r\nTotal: ");
    VEMAR_DEBUG(ulong, packet->geiger.total);
    VEMAR_DEBUG(str, "\r\nDelta: ");
    VEMAR_DEBUG(ulong, packet->geiger.delta);
    VEMAR_DEBUG(str, "\r\nCPM: ");
    VEMAR_DEBUG(ulong, packet->geiger.cpm);
    VEMAR_DEBUG(str, "\r\n");

    return (TRUE);
}
