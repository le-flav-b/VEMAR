#include <radio.h>
#include <spi.h>

#include "module.h"
#include "motor.h"
#include "data_save.h"
#include "led.h"

#define PIN_RADIO_CE PIN_PD2
#define PIN_RADIO_CSN PIN_PD3

packet_t g_packet;
uint8_t g_module_en;

void CAR_handle_movement(void);
void CAR_read_and_transmit(uint8_t id, uint8_t addr,
    bool_t (*module)(uint8_t, packet_t*),
    uint8_t (*append)(packet_t*));

void setup(void)
{
#ifdef VEMAR_DEBUG_ENABLED
    SERIAL_init();
#endif
    _delay_ms(500);
    /* Hold radio CSN high before any SPI activity — prevents NRF24L01 from
       latching SD card MISO traffic while its CSN is not yet configured */
    DDRD  |= (1 << PD3);
    PORTD |= (1 << PD3);
    uint8_t sd_res = 1;
    for (uint8_t i = 0; i < 5 && sd_res != 0; i++) {
        sd_res = sd_prepare();
    }
    /* SD_init leaves SPI enabled; SPI_init skips if SPE is set, so reset
       first so RADIO_init gets the correct clock speed (F_CPU/4) */
    SPI_reset();
    RADIO_init(PIN_RADIO_CE, PIN_RADIO_CSN);
    led_init();
    motor_init();
    i2c_init();
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
            CAR_read_and_transmit(PACKET_ID_ATM, ATMOSPHERE_ADDRESS, ATMOSPHERE_fill_packet, sd_append_atmosphere);
            data_type = 1;
        }
        else if (1 == data_type)
        {
            CAR_read_and_transmit(PACKET_ID_GAS, GAS_ADDRESS, GAS_fill_packet, sd_append_gas);
            data_type = 2;
        }
        else
        {
            CAR_read_and_transmit(PACKET_ID_GMC, GEIGER_ADDRESS, GEIGER_fill_packet, sd_append_radioactivity);
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

	motor_left_set(g_packet.car.ly);
	motor_right_set(g_packet.car.ry);
	sd_set_save(g_packet.car.save);
}

void CAR_read_and_transmit(uint8_t id, uint8_t addr,
    bool_t (*module)(uint8_t, packet_t*),
    uint8_t (*append)(packet_t*))
{
    if (module(addr, &g_packet)) {
        BIT_set(g_packet.header.module, BIT(id));
        if (!RADIO_write(g_packet.buffer, PACKET_SIZE)) {
            VEMAR_DEBUG(str, "module ID: ");
            VEMAR_DEBUG(int, id);
            VEMAR_DEBUG(str, " failed to transmit\r\n");
        }
        append(&g_packet);
    }
    else
    {
        BIT_clear(g_packet.header.module, BIT(id));
    }
}

/*
int main() // include motor.h
{
	motor_init();

	while (1)
	{
		motor_left_set(-100);
		motor_right_set(100);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);

		motor_left_set(-255);
		motor_right_set(255);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);

		motor_left_set(100);
		motor_right_set(-100);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);

		motor_left_set(255);
		motor_right_set(-255);
		_delay_ms(5000);

		motor_left_set(0);
		motor_right_set(0);
		_delay_ms(2000);
	}
}
*/