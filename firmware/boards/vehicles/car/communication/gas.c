#include "module.h"

#define GAS_SIZE 14 /**< 2-byte length header + 12 bytes payload */

// Packet offsets
#define IDX_CO2 2
#define IDX_CO 4
#define IDX_NH3 6
#define IDX_NO2 8
#define IDX_O2 10
#define IDX_TEMP 12
#define IDX_STATUS 13

#define CO2_TEMP_OFFSET 44 // subtract from raw TEMP byte to get °C

// STATUS bits
#define STATUS_CO2_VALID 0x01
#define STATUS_CO2_PREHEATING 0x02
#define STATUS_CO2_RX_SEEN 0x04
#define STATUS_CO2_FRAME_SEEN 0x08
#define STATUS_CO2_UART_ERR 0x10
#define STATUS_CO2_RX_EDGE 0x20
#define STATUS_CO2_CMD_SENT 0x40

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
    packet->gas.temp = (int8_t)(buffer[IDX_TEMP]) - CO2_TEMP_OFFSET;
    packet->gas.status = buffer[IDX_STATUS];

    VEMAR_DEBUG(str, "== Gas Readings ==\r\nCO2: ");
    VEMAR_DEBUG(uint, packet->gas.co2);
    VEMAR_DEBUG(str, (packet->gas.status & STATUS_CO2_VALID)
                         ? " ppm (CRC ok)"
                         : " ppm (CRC pending)");
    VEMAR_DEBUG(str, "\r\nCO2 status byte: 0x");
    VEMAR_DEBUG(hex, packet->gas.status, 2);
    VEMAR_DEBUG(str, "\r\nCO2 UART rx_seen=");
    VEMAR_DEBUG(bool, packet->gas.status &STATUS_CO2_RX_SEEN);
    VEMAR_DEBUG(str, ", frame_seen=");
    VEMAR_DEBUG(bool, packet->gas.status &STATUS_CO2_FRAME_SEEN);
    VEMAR_DEBUG(str, ", uart_err=");
    VEMAR_DEBUG(bool, packet->gas.status &STATUS_CO2_UART_ERR);
    VEMAR_DEBUG(str, ", rx_edge=");
    VEMAR_DEBUG(bool, packet->gas.status &STATUS_CO2_RX_EDGE);
    VEMAR_DEBUG(str, ", cmd_send=");
    VEMAR_DEBUG(bool, packet->gas.status &STATUS_CO2_CMD_SENT);
    VEMAR_DEBUG(str, "\r\nTemp(CO2 sensor): ");
    VEMAR_DEBUG(int, packet->gas.temp);
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
