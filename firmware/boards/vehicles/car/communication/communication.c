#include <radio.h>
#include <i2c.h>
#include <util/packet.h>

#include "atmosphere.h"

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

void CAR_tx_module(uint8_t mod)
{
    g_packet.header.id = PACKET_ID_CAR;
    g_packet.header.module = BIT(mod);
    RADIO_write(g_packet.buffer, PACKET_SIZE);
}

void setup(void)
{
#ifdef VEMAR_DEBUG_ENABLED
    SERIAL_init();
#endif
    RADIO_init(PIN_RADIO_CE, PIN_RADIO_CSN);
    i2c_init();
    _delay_ms(100);
    BME_init();
    _delay_ms(500);

    VEMAR_DEBUG(str, "setup done\r\n");
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
    // delay(1000);
    if (++count > 10000)
    {
        count = 0;
        if (0 == data_type)
        {
            // CAR_tx_module(PACKET_ID_GAS);
            CAR_read_atmosphere();
            data_type = 1;
        }
        else
        {
            // CAR_tx_module(PACKET_ID_GMC);
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
    if (!BME_do_read()) { VEMAR_DEBUG(str, "error while reading bme\r\n"); }

    g_packet.atmosphere.pm25 = BME_pm25();
    g_packet.atmosphere.pm10 = BME_pm10();

    g_packet.atmosphere.temperature = BME_temperature();
    g_packet.atmosphere.pressure  = BME_pressure();
    g_packet.atmosphere.humidity = BME_humidity();

    g_packet.atmosphere.id = PACKET_ID_ATM;

    if (RADIO_write(g_packet.buffer, PACKET_SIZE))
    {
    VEMAR_DEBUG(str, "=== Atmosphere Readings ===\r\nPM2.5: ");
    VEMAR_DEBUG(uint, g_packet.atmosphere.pm25);
    VEMAR_DEBUG(str, " ug/m3\r\nPM10: ");
    VEMAR_DEBUG(uint, g_packet.atmosphere.pm10);
    VEMAR_DEBUG(str, " ug/m3\r\nTemperature: ");
    VEMAR_DEBUG(int, g_packet.atmosphere.temperature);
    VEMAR_DEBUG(str, " C\r\nPressure: ");
    VEMAR_DEBUG(uint, g_packet.atmosphere.pressure);
    VEMAR_DEBUG(str, " Pa\r\nHumidity: ");
    VEMAR_DEBUG(uint, g_packet.atmosphere.humidity);
    VEMAR_DEBUG(str, " %RH\r\n");

    } else {
        VEMAR_DEBUG(str, "Atmosphere packet failed to send\r\n");
    }

    // float pm25_ugm3 = (float)pm25_raw / 10.0f;
    // float pm10_ugm3 = (float)pm10_raw / 10.0f;
    // float temp_c      = (float)temp_centi / 100.0f;
    // float pressure_pa = (float)pressure_q24_8 / 25600.0f;
    // float humidity_rh = (float)humidity_q22_10 / 1024.0f;

    // VEMAR_DEBUG(str, "SDS frames: ");
    // uint8_t sds_frames = buffer[PM_PACKET_SIZE];
    // uint8_t sds_rx     = buffer[PM_PACKET_SIZE + 1];
    // VEMAR_DEBUG(uint, sds_frames);
    // VEMAR_DEBUG(str, " \r\nRX bytes: ");
    // VEMAR_DEBUG(uint, sds_rx);
    // VEMAR_DEBUG(str, "\r\n");
}

void CAR_read_gas(void)
{
    VEMAR_DEBUG(str, "read gas\r\n");
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
    // else
    {
        if (BIT_is_set(g_packet.gas.status, STATUS_CO2_PREHEATING))
        {
            VEMAR_DEBUG(str, "[CO2 sensor preheating - < 60s uptime]");
        }
        VEMAR_DEBUG(str, "CO2: ");
        VEMAR_DEBUG(uint, g_packet.gas.co2);
        VEMAR_DEBUG(str, (g_packet.gas.status & STATUS_CO2_VALID)
                                ? " ppm (CRC ok)"
                                : " ppm (CRC pending)");
        VEMAR_DEBUG(str, "\r\nCO2 status byte: 0x");
        VEMAR_DEBUG(hex, g_packet.gas.status, 2);
        VEMAR_DEBUG(str, "\r\nCO2 UART rx_seen=");
        VEMAR_DEBUG(bool, g_packet.gas.status &STATUS_CO2_RX_SEEN);
        VEMAR_DEBUG(str, ", frame_seen=");
        VEMAR_DEBUG(bool, g_packet.gas.status &STATUS_CO2_FRAME_SEEN);
        VEMAR_DEBUG(str, ", uart_err=");
        VEMAR_DEBUG(bool, g_packet.gas.status &STATUS_CO2_UART_ERR);
        VEMAR_DEBUG(str, ", rx_edge=");
        VEMAR_DEBUG(bool, g_packet.gas.status &STATUS_CO2_RX_EDGE);
        VEMAR_DEBUG(str, ", cmd_send=");
        VEMAR_DEBUG(bool, g_packet.gas.status &STATUS_CO2_CMD_SENT);
        VEMAR_DEBUG(str, "\r\nTemp(CO2 sensor): ");
        VEMAR_DEBUG(int, g_packet.gas.temp);
        VEMAR_DEBUG(str, "\r\nCO:  ");
        VEMAR_DEBUG(uint, g_packet.gas.co);
        VEMAR_DEBUG(str, "\r\nNH3: ");
        VEMAR_DEBUG(uint, g_packet.gas.nh3);
        VEMAR_DEBUG(str, "\r\nNO2: ");
        VEMAR_DEBUG(uint, g_packet.gas.no2);
        VEMAR_DEBUG(str, "\r\nO2:  ");
        VEMAR_DEBUG(uint, g_packet.gas.o2);
        VEMAR_DEBUG(str, "\r\n---\r\n");
    }
#endif
}
