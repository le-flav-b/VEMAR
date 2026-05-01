#include <radio.h>

#include "module.h"

#define PIN_RADIO_CE PIN_PD2
#define PIN_RADIO_CSN PIN_PD3

packet_t g_packet;
uint8_t g_module_en;

void CAR_handle_movement(void);
void CAR_read_and_transmit(uint8_t id, uint8_t addr,
    bool_t (*module)(uint8_t, packet_t*));

void setup(void)
{
#ifdef VEMAR_DEBUG_ENABLED
    SERIAL_init();
#endif

    RADIO_init(PIN_RADIO_CE, PIN_RADIO_CSN);

    i2c_init();
    _delay_ms(100);
    if (!ATMOSPHERE_init())
    {
        VEMAR_DEBUG(str, "atmosphere module init failed\r\n");
    }
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
            CAR_read_and_transmit(PACKET_ID_ATM, ATMOSPHERE_ADDRESS, ATMOSPHERE_fill_packet);

            data_type = 1;
        }
        else if (1 == data_type)
        {
            CAR_read_and_transmit(PACKET_ID_GAS, GAS_ADDRESS, GAS_fill_packet);
            data_type = 2;
        }
        else
        {
            CAR_read_and_transmit(PACKET_ID_GMC, GEIGER_ADDRESS, GEIGER_fill_packet);
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

void CAR_read_and_transmit(uint8_t id, uint8_t addr,
    bool_t (*module)(uint8_t, packet_t*))
{
    if (module(addr, &g_packet)) {
        BIT_set(g_packet.header.module, BIT(id));
        if (!RADIO_write(g_packet.buffer, PACKET_SIZE)) {
            VEMAR_DEBUG(str, "module ID: ");
            VEMAR_DEBUG(int, id);
            VEMAR_DEBUG(str, " failed to transmit\r\n");
        }
    }
    else
    {
        BIT_clear(g_packet.header.module, BIT(id));
    }
}
