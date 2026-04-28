#include <radio.h>
#include <i2c.h>
#include <util/packet.h>

#define PIN_RADIO_CE PIN_PD2
#define PIN_RADIO_CSN PIN_PD3

/**
 * @brief Combine 2 bytes into 16-bit value
 * @param _high High byte
 * @param _low Low byte
 */
#define U8HL_TO_U16BIT(_high, _low) ((uint16_t)((_high) << 8) | (_low))

#ifdef VEMAR_DEBUG_ENABLED
#include <serial.h>
#define VEMAR_DEBUG(_type, ...) SERIAL_print(_type, __VA_ARGS__)
#else
#define VEMAR_DEBUG(_type, ...)
#endif

packet_t g_packet;
uint8_t g_module_en;

void CAR_handle_movement(void);
void CAR_read_atmosphere(void);
void CAR_read_gas(void);

static inline void CAR_enable_module(uint8_t module_id)
{
    BIT_set(g_module_en, BIT(module_id));
}

static inline void CAR_disable_module(uint8_t module_id)
{
    BIT_clear(g_module_en, BIT(module_id));
}

void setup(void)
{
#ifdef VEMAR_DEBUG_ENABLED
    SERIAL_init();
#endif
    RADIO_init(PIN_RADIO_CE, PIN_RADIO_CSN);
}

void loop(void)
{
    static uint32_t count = 0;
    static uint8_t data_type = 0;

    if (RADIO_read(g_packet.buffer, PACKET_SIZE))
    {
        if (PACKET_ID_CAR == g_packet.header.id)
        {
            CAR_handle_movement();
        }
    }
    if (++count > 10000)
    {
        count = 0;
        if (0 == data_type)
        {
            CAR_read_atmosphere();
            data_type = 1;
        }
        else
        {
            CAR_read_gas();
            data_type = 0;
        }
    }
}

void CAR_handle_movement(void)
{
    /** @todo handle car movement */

    VEMAR_DEBUG(str, "-- from controller\r\n");
    VEMAR_DEBUG(str, "LX: ");
    VEMAR_DEBUG(int, g_packet.car.lx);
    VEMAR_DEBUG(str, "; LY: ");
    VEMAR_DEBUG(int, g_packet.car.ly);
    VEMAR_DEBUG(str, "; LB: ");
    VEMAR_DEBUG(bool, g_packet.car.lb);
    VEMAR_DEBUG(str, "\r\nRX: ");
    VEMAR_DEBUG(int, g_packet.car.rx);
    VEMAR_DEBUG(str, "; RY: ");
    VEMAR_DEBUG(int, g_packet.car.ry);
    VEMAR_DEBUG(str, "; RB: ");
    VEMAR_DEBUG(bool, g_packet.car.rb);
    VEMAR_DEBUG(str, "\r\nPotentiometer: ");
    VEMAR_DEBUG(uint, g_packet.car.pot);
    VEMAR_DEBUG(str, "\r\n--------\r\n");
}

void CAR_read_atmosphere(void)
{
    /** @todo Retrieve atmosphere data */

    static int t;
    static int h = 50;
    static int p;

    g_packet.header.id = PACKET_ID_ATM;
    g_packet.atmosphere.temperature = t;
    g_packet.atmosphere.humidity = h;
    g_packet.atmosphere.pressure = p;

    if (RADIO_write(g_packet.buffer, PACKET_SIZE))
    {
        VEMAR_DEBUG(str, "ID: ");
        VEMAR_DEBUG(int, g_packet.header.id);
        VEMAR_DEBUG(str, "; n: ");
        VEMAR_DEBUG(int, g_packet.atmosphere.pressure);
        VEMAR_DEBUG(str, "; t: ");
        VEMAR_DEBUG(int, g_packet.atmosphere.temperature);
        VEMAR_DEBUG(str, "; h: ");
        VEMAR_DEBUG(int, g_packet.atmosphere.humidity);
        VEMAR_DEBUG(str, "\r\n----------\r\n");

        t = (t + 12) % 1000;
        h = (h * 2 + 1) % 1000;
        p = (p + 1) % 1000;
    }
}

void CAR_read_gas(void)
{
    uint8_t buffer[I2C_BUFFER_SIZE] = {0};
    if (i2c_read_packet(GAS_ADDRESS, buffer))
    {
        VEMAR_DEBUG(str, "I2C error\r\n");
        // return;
    }
    g_packet.header.id = PACKET_ID_GAS;
    g_packet.gas.co2 = U8HL_TO_U16BIT(buffer[IDX_CO2], buffer[IDX_CO2 + 1]);
    g_packet.gas.co = U8HL_TO_U16BIT(buffer[IDX_CO], buffer[IDX_CO + 1]);
    g_packet.gas.nh3 = U8HL_TO_U16BIT(buffer[IDX_NH3], buffer[IDX_NH3 + 1]);
    g_packet.gas.no2 = U8HL_TO_U16BIT(buffer[IDX_NO2], buffer[IDX_NO2 + 1]);
    g_packet.gas.o2 = U8HL_TO_U16BIT(buffer[IDX_O2], buffer[IDX_O2 + 1]);
    g_packet.gas.temp = (int8_t)(buffer[IDX_TEMP]) - CO2_TEMP_OFFSET;
    g_packet.gas.status = buffer[IDX_STATUS];

    if (!RADIO_write(g_packet.buffer, PACKET_SIZE))
    {
        VEMAR_DEBUG(str, "gas transmission failed\r\n");
    }

#ifdef VEMAR_DEBUG_ENABLED
    else
    {
        if (BIT_is_set(g_packet.gas.status, STATUS_CO2_PREHEATING))
        {
            SERIAL_println(str, "[CO2 sensor preheating - < 60s uptime]");
        }
        SERIAL_print(str, "CO2: ");
        SERIAL_print(uint, g_packet.gas.co2);
        SERIAL_println(str, (g_packet.gas.status & STATUS_CO2_VALID)
                                ? " ppm (CRC ok)"
                                : " ppm (CRC pending)");
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
#endif
}
