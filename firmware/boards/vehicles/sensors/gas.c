#include <serial.h>
#include <radio.h>
#include <i2c.h>
#include <util/packet.h>
#include <util/gas.h>

packet_t g_packet;

#define U8HL_TO_U16BIT(_high, _low) ((uint16_t)((_high) << 8) | (_low))

void display_gas(void)
{
        if (BIT_is_set(g_packet.gas.status, STATUS_CO2_PREHEATING))
    {
        SERIAL_println(str, "[CO2 sensor preheating - < 60s uptime]");
    }

    SERIAL_print(str, "CO2: ");
    SERIAL_print(uint, g_packet.gas.co2);
    SERIAL_println(str, (g_packet.gas.status & STATUS_CO2_VALID) ? " ppm (CRC ok)" : " ppm (CRC pending)");
    SERIAL_print(str, "CO2 status byte: 0x");
    SERIAL_println(hex, g_packet.gas.status, 2);
    SERIAL_print(str, "CO2 UART rx_seen=");
    SERIAL_print(bool, g_packet.gas.status &STATUS_CO2_RX_SEEN);
    SERIAL_print(str, ", frame_seen=");
    SERIAL_print(bool, g_packet.gas.status &STATUS_CO2_FRAME_SEEN);
    SERIAL_print(str, ", uart_err=");
    SERIAL_print(bool, g_packet.gas.status &STATUS_CO2_UART_ERR);
    SERIAL_print(str, ", rx_edge=");
    SERIAL_print(bool, g_packet.gas.status &STATUS_CO2_RX_EDGE);
    SERIAL_print(str, ", cmd_send=");
    SERIAL_println(bool, g_packet.gas.status &STATUS_CO2_CMD_SENT);
    SERIAL_print(str, "Temp(CO2 sensor): ");
    SERIAL_println(int, g_packet.gas.temp);
    SERIAL_print(str, "CO:  ");
    SERIAL_println(uint, g_packet.gas.co);
    SERIAL_print(str, "NH3: ");
    SERIAL_println(uint, g_packet.gas.nh3);
    SERIAL_print(str, "NO2: ");
    SERIAL_println(uint, g_packet.gas.no2);
    SERIAL_print(str, "O2:  ");
    SERIAL_println(uint, g_packet.gas.o2);
    SERIAL_println(str, "---");
}

void setup(void)
{
    SERIAL_init();
    RADIO_init(PIN_PB0, PIN_PB1);
    i2c_init();
    SERIAL_println(str, "Gasses master ready");
}

void loop(void)
{
    SERIAL_println(str, "new reading");
    uint8_t buffer[I2C_BUFFER_SIZE] = {0};
    if (i2c_read_packet(GAS_ADDRESS, buffer))
    {
        SERIAL_println(str, "I2C error");
        return;
    }
    SERIAL_println(str, "parse data");
    g_packet.gas.id = PACKET_ID_GAS;
    g_packet.gas.co2 = U8HL_TO_U16BIT(buffer[IDX_CO2], buffer[IDX_CO2 + 1]);
    g_packet.gas.co = U8HL_TO_U16BIT(buffer[IDX_CO], buffer[IDX_CO + 1]);
    g_packet.gas.nh3 = U8HL_TO_U16BIT(buffer[IDX_NH3], buffer[IDX_NH3 + 1]);
    g_packet.gas.no2 = U8HL_TO_U16BIT(buffer[IDX_NO2], buffer[IDX_NO2 + 1]);
    g_packet.gas.o2 = U8HL_TO_U16BIT(buffer[IDX_O2], buffer[IDX_O2 + 1]);
    g_packet.gas.temp = (int8_t)(buffer[IDX_TEMP]) - CO2_TEMP_OFFSET;
    g_packet.gas.status = buffer[IDX_STATUS];
    RADIO_write(g_packet.data, PACKET_SIZE);

    display_gas();
    delay(2000);
}
