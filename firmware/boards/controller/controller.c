#include "controller.h"
#include "packet.h"
#include "serial.h"


controller_t g_controller;
packet_t g_packet;

/**
 * @brief VEMAR Radio Status
 * - __bit-7__: Status Change, mask 0x80
 * - __bit-6__: Disconnect, mask 0x40
 * - __[5:0]__: Counter, mask 0x3F
 */
byte_t g_radio_status;

/** @brief Radio Status Change flag */
#define _RADIO_STATUS_CHANGE(_status) BIT_read(_status, 0x80)

/** @brief Radio Disconnect flag */
#define _RADIO_DISCONNECT(_status) BIT_read(_status, 0x40)

/** @brief Radio Disconnect Counter */
#define _RADIO_COUNTER(_status) BIT_read(_status, 0x3F)

/** @brief Radio is connected */
#define _RADIO_IS_STATUS_CONNECTED(_status) BIT_is_clear(_status, 0x40)

/** @brief Radio signal is lost */
#define _RADIO_IS_STATUS_DISCONNECTED(_status) BIT_is_set(_status, 0x40)

/** @brief Change radio status from Disconnected to Connected */
#define _RADIO_DIS_TO_CON(_status) \
    ((BIT_read(_status, 0x40) ^ 0x40) << 1)

/** @brief Change radio status from Connected to Disconnected */
#define _RADIO_CON_TO_DIS(_status) \
    (((BIT_read(_status, 0x40) ^ 0x40) << 1) | 0x40)

/** @brief Maximum retries before setting Disconnect flag */
#define _RADIO_MAX_RETRY 0x10

/** @brief Radio Transmission is enabled */
#define _CONTROLLER_TX_ENABLED 0x02

/** @brief Radio Reception is enabled */
#define _CONTROLLER_RX_ENABLED 0x01

/** @brief Delay for reception */
#define _CONTROLLER_RX_DELAY 8000U

/**
 * @brief Return the current operation mode: RX only, TX only, or both
 */
static inline byte_t _CONTROLLER_status(void);

/**
 * @brief Increase the disconnection counter, set the Disconnect flag if
 * it reaches `_RADIO_MAX_RETRY`
 */
static inline void _CONTROLLER_disconnect(void);

// initialization
void setup(void)
{
#ifdef VEMAR_DEBUG_ENABLED
    SERIAL_init();
#endif

    CONTROLLER_DEBUG(str, "start setup");
    CONTROLLER_init();
    RADIO_init(PIN_RADIO_CE, PIN_RADIO_CSN);
    TFT_init(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
    CONTROLLER_setup_display();
    CONTROLLER_DEBUG(str, "end setup");
}

// main loop
void loop(void)
{
    byte_t status = _CONTROLLER_status();
    if (BIT_is_set(status, _CONTROLLER_TX_ENABLED))
    {
        CONTROLLER_write();
    } // if TX enabled
    if (BIT_is_set(status, _CONTROLLER_RX_ENABLED))
    {
        CONTROLLER_read();
    } // if RX enabled
    CONTROLLER_update_connection();
}

// CONTROLLER_init
void CONTROLLER_init(void)
{
    g_controller.tgl.up = BUTTON_new(PIN_TOGGLE_UP, BUTTON_ONHOLD);
    g_controller.tgl.down = BUTTON_new(PIN_TOGGLE_DOWN, BUTTON_ONHOLD);
    g_controller.btn1 = BUTTON_new(PIN_BUTTON1, BUTTON_ONPRESS);
    g_controller.btn2 = BUTTON_new(PIN_BUTTON2, BUTTON_ONPRESS);
    g_controller.pot = ANALOG_new(PIN_POTENTIOMETER);
    g_controller.jright = JOYSTICK_new(PIN_JOY_RX, PIN_JOY_RY, PIN_JOY_RB);
    g_controller.jleft = JOYSTICK_new(PIN_JOY_LX, PIN_JOY_LY, PIN_JOY_LB);
    g_controller.led = LED_new(PIN_LED);
}

// CONTROLLER_setup_display
void CONTROLLER_setup_display(void)
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

// CONTROLLER_update_atmosphere
void CONTROLLER_update_atmosphere(void)
{
    char *str;

    str = UTIL_itoa_decimal(g_packet.atmosphere.temperature, 4);
    TFT_print_str(COL2, ROW1, str);

    str = UTIL_itoa_decimal(g_packet.atmosphere.humidity, 4);
    TFT_print_str(COL2, ROW2, str);

    str = UTIL_itoa_decimal(g_packet.atmosphere.pressure, 4);
    TFT_print_str(COL2, ROW3, str);
}

// CONTROLLER_update_connection
void CONTROLLER_update_connection(void)
{
    if (_RADIO_STATUS_CHANGE(g_radio_status))
    {
        if (_RADIO_IS_STATUS_CONNECTED(g_radio_status))
        {
            TFT_print_str(COL1, ROW13, "            ");
        } // if Disconnected -> Connected
        else if (_RADIO_IS_STATUS_DISCONNECTED(g_radio_status))
        {
            TFT_print_str(COL1, ROW13, "Disconnected");
        } // if Connected -> Disconnected
    } // if Status Change flag is set
}

// CONTROLLER_read
void CONTROLLER_read(void)
{
    for (uint16_t r = 0; r < _CONTROLLER_RX_DELAY; ++r)
    {
        if (RADIO_read(g_packet.data, PACKET_SIZE))
        {
            if (PACKET_ID_ATM == g_packet.header.id)
            {
                CONTROLLER_update_atmosphere();
            }
            g_radio_status = _RADIO_DIS_TO_CON(g_radio_status);
            break;
        } // has received a packet
    }
    _CONTROLLER_disconnect();
    // if (_RADIO_COUNTER(g_radio_status) < _RADIO_MAX_RETRY)
    // {
    //     ++g_radio_status;
    // } // increase disconnection count
    // else
    // {
    //     g_radio_status = _RADIO_CON_TO_DIS(g_radio_status) | _RADIO_MAX_RETRY;
    // } // disconnected and display
}

// CONTROLLER_write
void CONTROLLER_write(void)
{
    g_packet.header.id = PACKET_ID_CAR;
    g_packet.car.velocity = ANALOG_read(g_controller.pot);
    if (RADIO_write(g_packet.data, PACKET_SIZE))
    {
        g_radio_status = _RADIO_DIS_TO_CON(g_radio_status);
    }
    else
    {
        _CONTROLLER_disconnect();
        // if (_RADIO_COUNTER(g_radio_status) < _RADIO_MAX_RETRY)
        // {
        //     ++g_radio_status;
        // } // increase disconnection count
        // else
        // {
        //     g_radio_status = _RADIO_CON_TO_DIS(g_radio_status) | _RADIO_MAX_RETRY;
        // } // disconnected and display
    }
}

byte_t _CONTROLLER_status(void)
{
    if (BUTTON_is_active(&(g_controller.tgl.up)))
    {
        return ((byte_t)_CONTROLLER_TX_ENABLED);
    } // enable RX
    else if (BUTTON_is_active(&(g_controller.tgl.down)))
    {
        return ((byte_t)_CONTROLLER_RX_ENABLED);
    } // enable TX
    return ((byte_t)(_CONTROLLER_TX_ENABLED | _CONTROLLER_RX_ENABLED));
}

void _CONTROLLER_disconnect(void)
{
    if (_RADIO_COUNTER(g_radio_status) < _RADIO_MAX_RETRY)
    {
        ++g_radio_status;
    } // increase disconnection count
    else
    {
        g_radio_status = _RADIO_CON_TO_DIS(g_radio_status) | _RADIO_MAX_RETRY;
    } // disconnected and display
}
