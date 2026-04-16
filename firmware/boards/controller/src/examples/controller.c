#include "controller.h"
#include "packet.h"
#include "serial.h"

controller_t controller;
packet_t packet;

/**
 * @brief VEMAR Radio Status
 * bit-7: Status Change
 * bit-6: Disconnect
 * [5:0]: Counter
 */
byte_t g_radio_status;

#define VEMAR_RADIO_STATUS_CHANGE_MASK 0x80
#define VEMAR_RADIO_DISCONNECT_MASK 0x40
#define VEMAR_RADIO_COUNTER_MASK 0x3F
#define VEMAR_RADIO_MAX_RETRY 0x10

static inline void display_temperature(int value);
static inline void display_humidity(int value);

void setup_tft(void);

void setup(void)
{
    SERIAL_init();
    CONTROLLER_init();
    RADIO_init(PIN_RADIO_CE, PIN_RADIO_CSN);
    TFT_init(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
    setup_tft();
    SERIAL_println(str, "end of setup");
}

void loop(void)
{
    for (length_t r = 0; r < 100; ++r)
    {
        CONTROLLER_receive();
    }
    if (BUTTON_is_active(&(controller.toggle.up)))
    {
        CONTROLLER_send_movement();
    }
}

void setup_tft(void)
{
    TFT_set_mode(TFT_LANDSCAPE, TFT_NORMAL, TFT_NORMAL);
    TFT_setup_text(TFT_TEXT_S, 1, RGB16_WHITE, RGB16_BLACK);
    TFT_fill_screen(RGB16_BLACK);
    TFT_print_str(COL1, ROW1, "Temperature: ");
    TFT_print_str(COL3, ROW1, "C");
    TFT_print_str(COL1, ROW2, "Humidity   : ");
    TFT_print_str(COL3, ROW2, "%");
    TFT_print_str(COL1, ROW3, "Pressure   : ");
    TFT_print_str(COL3, ROW3, "hPa");
}

void display_temperature(int value)
{
    char *str = UTIL_itoa_decimal(value, 4);
    TFT_print_str(COL2, ROW1, str);
}

void display_humidity(int value)
{
    char *str = UTIL_itoa_decimal(value, 4);
    TFT_print_str(COL2, ROW2, str);
}

void display_pressure(int value)
{
    char *str = UTIL_itoa_decimal(value, 4);
    TFT_print_str(COL2, ROW3, str);
}

void disconnected(void)
{
    if (BIT_is_set(g_radio_status, (VEMAR_RADIO_DISCONNECT_MASK | VEMAR_RADIO_STATUS_CHANGE_MASK)))
    {
        TFT_print_str(COL1, ROW10, "Disconnected");
    }
    else if (BIT_is_set(g_radio_status, VEMAR_RADIO_STATUS_CHANGE_MASK))
    {
        TFT_print_str(COL1, ROW10, "            ");
    }
}

void CONTROLLER_init(void)
{
    SERIAL_println(str, "controller init");
    controller.toggle.up = BUTTON_new(PIN_TOGGLE_UP, BUTTON_ONHOLD);
    controller.toggle.down = BUTTON_new(PIN_TOGGLE_DOWN, BUTTON_ONHOLD);
    controller.btn1 = BUTTON_new(PIN_BUTTON1, BUTTON_ONPRESS);
    controller.btn2 = BUTTON_new(PIN_BUTTON2, BUTTON_ONPRESS);
    controller.potentiometer = ANALOG_new(PIN_POTENTIOMETER);
    controller.right = JOYSTICK_new(PIN_JOY_RX, PIN_JOY_RY, PIN_JOY_RB);
    controller.left = JOYSTICK_new(PIN_JOY_LX, PIN_JOY_LY, PIN_JOY_LB);
    controller.led = LED_new(PIN_LED);
}

void CONTROLLER_send_movement(void)
{
    packet.header.id = PACKET_ID_CAR;
    packet.car.velocity = JOYSTICK_x(&(controller.right));
    if (RADIO_write(packet.data, PACKET_SIZE))
    {
        g_radio_status = ((BIT_read(g_radio_status, VEMAR_RADIO_DISCONNECT_MASK) ^
                           VEMAR_RADIO_DISCONNECT_MASK)
                          << 1);
    }
    else
    {
        if (BIT_read(g_radio_status, VEMAR_RADIO_COUNTER_MASK) <
            VEMAR_RADIO_MAX_RETRY)
        {
            ++g_radio_status;
        }
        else
        {
            g_radio_status = ((g_radio_status ^ VEMAR_RADIO_DISCONNECT_MASK)
                              << 1) |
                             VEMAR_RADIO_DISCONNECT_MASK;
        }
    }
    disconnected();
}

// void CONTROL_debug(const char *label)
// {
//     SERIAL_print(str, label);
//     SERIAL_print(str, " >>>> CS:");
//     SERIAL_print(int, (PIN_read(PIN_TFT_CS)));
//     SERIAL_print(str, ";DC:");
//     SERIAL_print(int, (PIN_read(PIN_TFT_DC)));
//     SERIAL_print(str, ";RST:");
//     SERIAL_print(int, (PIN_read(PIN_TFT_RST)));
//     SERIAL_print(str, ";CE:");
//     SERIAL_print(int, (PIN_read(PIN_RADIO_CE)));
//     SERIAL_print(str, ";CSN:");
//     SERIAL_println(int, (PIN_read(PIN_RADIO_CSN)));
// }

void CONTROLLER_receive(void)
    {
    if (RADIO_read(packet.data, PACKET_SIZE))
    {
        if (PACKET_ID_THP == packet.header.id)
        {
            display_temperature(packet.sensor.temperature);
            display_humidity(packet.sensor.humidity);
            display_pressure(packet.sensor.pressure);
        }
    }
}
