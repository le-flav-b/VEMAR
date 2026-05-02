#include "module.h"

/**
 * @brief Combine 2 bytes into 16-bit value
 * @param _high High byte
 * @param _low Low byte
 */
#define U8HL_TO_U16BIT(_high, _low) ((uint16_t)((_high) << 8) | (_low))

//------------------------------------------------------------------------------
// Gas
//------------------------------------------------------------------------------

bool_t GAS_fill_packet(uint8_t addr, packet_t *packet)
{
    uint8_t buffer[I2C_BUFFER_SIZE] = {0};

    if (i2c_read_packet(addr, buffer))
    {
        return (FALSE);
    }

    packet->gas.id = PACKET_ID_GAS;
    packet->gas.co2 = U8HL_TO_U16BIT(buffer[IDX_CO2], buffer[IDX_CO2 + 1]);
    packet->gas.co = U8HL_TO_U16BIT(buffer[IDX_CO], buffer[IDX_CO + 1]);
    packet->gas.nh3 = U8HL_TO_U16BIT(buffer[IDX_NH3], buffer[IDX_NH3 + 1]);
    packet->gas.no2 = U8HL_TO_U16BIT(buffer[IDX_NO2], buffer[IDX_NO2 + 1]);
    packet->gas.o2 = U8HL_TO_U16BIT(buffer[IDX_O2], buffer[IDX_O2 + 1]);
    packet->gas.temp = 0;
    packet->gas.status = 0;

    VEMAR_DEBUG(str, "== Gas Readings ==\r\nCO2: ");
    VEMAR_DEBUG(uint, packet->gas.co2);
    VEMAR_DEBUG(str, " ppm");
    VEMAR_DEBUG(str, "\r\nCO:  ");
    VEMAR_DEBUG(uint, packet->gas.co);
    VEMAR_DEBUG(str, "\r\nNH3: ");
    VEMAR_DEBUG(uint, packet->gas.nh3);
    VEMAR_DEBUG(str, "\r\nNO2: ");
    VEMAR_DEBUG(uint, packet->gas.no2);
    VEMAR_DEBUG(str, "\r\nO2:  ");
    VEMAR_DEBUG(uint, packet->gas.o2);
    VEMAR_DEBUG(str, "\r\n");

    return (TRUE);
}
