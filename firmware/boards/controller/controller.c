#include "controller.h"

controller_t g_controller;
packet_t g_packet;
packet_t g_packet_tx;
byte_t g_ctrl_mode;

/**
 * @brief VEMAR Radio Status
 * - __bit-7__: Status Change, mask 0x80
 * - __bit-6__: Disconnect, mask 0x40
 * - __[5:0]__: Counter, mask 0x3F
 */
byte_t g_radio_status;

#define _CONTROLLER_ADC_MAX 1023U

// Calibration of joystick
#define _ADC_CONVERT_LX(x) ((((int)x - 505) / 19) * 10)
#define _ADC_CONVERT_LY(x) ((((int)x - 518) / 20) * 10)
#define _ADC_CONVERT_RX(x) (((504 - (int)x) / 19) * 10)
#define _ADC_CONVERT_RY(x) (((538 - (int)x) / 20) * 10)

#define _CONTROLLER_MODE_DATA 0x01 /**< Display sensor data */
#define _CONTROLLER_MODE_MAP 0x02  /**< Display map */

#define _CONTROLLER_TX_ENABLED 0x02 /**< Radio Transmission is enabled */
#define _CONTROLLER_RX_ENABLED 0x01 /**< Radio Reception is enabled */
#define _CONTROLLER_RX_DELAY 8000U  /**< Delay for reception */

#define _DISPLAY_W 8U
#define _DISPLAY_H 8U

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

/**
 * @brief Return the current operation mode: RX only, TX only, or both
 */
static inline byte_t _CONTROLLER_status(void);

/**
 * @brief Increase the disconnection counter, set the `Disconnect` flag if
 * it reaches `_RADIO_MAX_RETRY`
 */
static inline void _CONTROLLER_disconnect(void);

/**
 * @brief Reset connection
 */
static inline void _CONTROLLER_reset_connection(void);

//------------------------------------------------------------------------------
// setup
//------------------------------------------------------------------------------
void setup(void)
{
#ifdef VEMAR_DEBUG_ENABLED
    SERIAL_init();
#endif

    CONTROLLER_DEBUG(str, "start setup\r\n");
    CONTROLLER_init();
    RADIO_init(PIN_RADIO_CE, PIN_RADIO_CSN);
    TFT_init(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
    TFT_set_mode(TFT_LANDSCAPE, TFT_INVERTED, TFT_INVERTED);
    TFT_setup_text(TFT_TEXT_S, 1, RGB16_WHITE, RGB16_BLACK);
    CONTROLLER_display_data();
    _CONTROLLER_reset_connection();
    CONTROLLER_DEBUG(str, "end setup\r\n");
}

//------------------------------------------------------------------------------
// main loop
//------------------------------------------------------------------------------
void loop(void)
{
    byte_t status = _CONTROLLER_status();
    if (BIT_is_set(status, _CONTROLLER_TX_ENABLED))
    {
        CONTROLLER_write();
        CONTROLLER_update_connection();
    } // if TX enabled
    if (BIT_is_set(status, _CONTROLLER_RX_ENABLED))
    {
        CONTROLLER_read();
        CONTROLLER_update_connection();
    } // if RX enabled

    if (BUTTON_is_active(&(g_controller.btn1)))
    {
        CONTROLLER_display_data();
        _CONTROLLER_reset_connection();
    } // button 1 pressed
    if (BUTTON_is_active(&(g_controller.btn2)))
    {
        CONTROLLER_display_map();
        CONTROLLER_update_map();
        _CONTROLLER_reset_connection();
    } // button 2 pressed
}

//------------------------------------------------------------------------------
// CONTROLLER_init
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// CONTROLLER_display_data
//------------------------------------------------------------------------------
void CONTROLLER_display_data(void)
{
    if (_CONTROLLER_MODE_DATA != g_ctrl_mode)
    {
        g_ctrl_mode = _CONTROLLER_MODE_DATA;
        TFT_fill_screen(RGB16_BLACK);
        TFT_print_str(COL1, ROW1, "Temperature: ");
        TFT_print_str(COL3, ROW1, "C");
        TFT_print_str(COL1, ROW2, "Humidity   : ");
        TFT_print_str(COL3, ROW2, "%");
        TFT_print_str(COL1, ROW3, "Pressure   : ");
        TFT_print_str(COL3, ROW3, "hPa");

        TFT_print_str(COL1, ROW4, "CO2        : ");
        TFT_print_str(COL3, ROW4, "ppm");
        TFT_print_str(COL1, ROW5, "CO         : ");
        TFT_print_str(COL3, ROW5, "(raw ADC)");
        TFT_print_str(COL1, ROW6, "NH3        : ");
        TFT_print_str(COL3, ROW6, "(raw ADC)");
        TFT_print_str(COL1, ROW7, "NO2        : ");
        TFT_print_str(COL3, ROW7, "(raw ADC)");
        TFT_print_str(COL1, ROW8, "O2         : ");
        TFT_print_str(COL3, ROW8, "(raw ADC)");
    }
}

//------------------------------------------------------------------------------
// CONTROLLER_display_map
//------------------------------------------------------------------------------
void CONTROLLER_display_map(void)
{
    if (_CONTROLLER_MODE_MAP != g_ctrl_mode)
    {
        g_ctrl_mode = _CONTROLLER_MODE_MAP;
        TFT_fill_screen(RGB16_BLACK);
    }
}

//------------------------------------------------------------------------------
// CONTROLLER_update_atmosphere
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// CONTROLLER_update_gas
//------------------------------------------------------------------------------
void CONTROLLER_update_gas(void)
{
    char *str;

    str = UTIL_itoa(g_packet.gas.co2, 4);
    TFT_print_str(COL2, ROW4, str);

    str = UTIL_itoa(g_packet.gas.co, 4);
    TFT_print_str(COL2, ROW5, str);

    str = UTIL_itoa(g_packet.gas.nh3, 4);
    TFT_print_str(COL2, ROW6, str);

    str = UTIL_itoa(g_packet.gas.no2, 4);
    TFT_print_str(COL2, ROW7, str);

    str = UTIL_itoa(g_packet.gas.o2, 4);
    TFT_print_str(COL2, ROW8, str);
}

void CONTROLLER_update_map(void)
{
    uint16_t row = 0;
    uint16_t line = 0xF001;
    for (length_t pos = 0; pos < 16; ++pos)
    {
        if (line & (1 << (15 - pos)))
        {
            TFT_fill_area(pos * _DISPLAY_W, row * _DISPLAY_H, _DISPLAY_W, _DISPLAY_H, RGB16_WHITE);
        }
    }
}

//------------------------------------------------------------------------------
// CONTROLLER_update_connection
//------------------------------------------------------------------------------
void CONTROLLER_update_connection(void)
{
    if (_RADIO_STATUS_CHANGE(g_radio_status))
    {
        if (_RADIO_IS_STATUS_CONNECTED(g_radio_status))
        {
            TFT_print_str(COL1, ROW_LAST, "signal: GOOD");
            g_radio_status = 0;
        } // if Disconnected -> Connected
        else if (_RADIO_IS_STATUS_DISCONNECTED(g_radio_status))
        {
            TFT_print_str(COL1, ROW_LAST, "signal: NONE");
        } // if Connected -> Disconnected
    } // if Status Change flag is set
}

//------------------------------------------------------------------------------
// CONTROLLER_read
//------------------------------------------------------------------------------
void CONTROLLER_read(void)
{
    bool_t signal = FALSE;

    for (uint16_t r = 0; r < _CONTROLLER_RX_DELAY; ++r)
    {
        if (RADIO_read(g_packet.buffer, PACKET_SIZE))
        {
            signal = TRUE;
            break;
        } // has received a packet
    }

    if (TRUE == signal)
    {
        if ((PACKET_ID_ATM == g_packet.header.id) &&
            (_CONTROLLER_MODE_DATA == g_ctrl_mode))
        {
            CONTROLLER_update_atmosphere();
        }
        else if ((PACKET_ID_GAS == g_packet.header.id) &&
                 (_CONTROLLER_MODE_DATA == g_ctrl_mode))
        {
            CONTROLLER_update_gas();
        }

        g_radio_status = _RADIO_DIS_TO_CON(g_radio_status);
        CONTROLLER_DEBUG(str, "packet received\r\n");
    }
    else
    {
        _CONTROLLER_disconnect();
    }
}

//------------------------------------------------------------------------------
// CONTROLLER_write
//------------------------------------------------------------------------------
void CONTROLLER_write(void)
{
    uint16_t pot = _CONTROLLER_ADC_MAX - ANALOG_read(g_controller.pot);

    int16_t joy_xl = _ADC_CONVERT_LX(ANALOG_read(g_controller.jleft.x));
    int16_t joy_yl = _ADC_CONVERT_LY(ANALOG_read(g_controller.jleft.y));
    bool_t joy_bl = BUTTON_is_active(&(g_controller.jleft.button));

    int16_t joy_xr = _ADC_CONVERT_RX(ANALOG_read(g_controller.jright.x));
    int16_t joy_yr = _ADC_CONVERT_RY(ANALOG_read(g_controller.jright.y));
    bool_t joy_br = BUTTON_is_active(&(g_controller.jright.button));

    if ((pot == g_packet_tx.car.pot) &&
        (joy_xl == g_packet_tx.car.lx) &&
        (joy_yl == g_packet_tx.car.ly) &&
        (joy_bl == g_packet_tx.car.lb) &&
        (joy_xr == g_packet_tx.car.rx) &&
        (joy_yr == g_packet_tx.car.ry) &&
        (joy_br == g_packet_tx.car.rb))
    {
        return;
    }
    g_packet_tx.header.id = PACKET_ID_CAR;
    g_packet_tx.car.pot = pot;
    g_packet_tx.car.lx = joy_xl;
    g_packet_tx.car.ly = joy_yl;
    g_packet_tx.car.lb = joy_bl;
    g_packet_tx.car.rx = joy_xr;
    g_packet_tx.car.ry = joy_yr;
    g_packet_tx.car.rb = joy_br;

    CONTROLLER_DEBUG(str, "LX: ");
    CONTROLLER_DEBUG(int, g_packet_tx.car.lx);
    CONTROLLER_DEBUG(str, "; LY: ");
    CONTROLLER_DEBUG(int, g_packet_tx.car.ly);
    CONTROLLER_DEBUG(str, "; LB: ");
    CONTROLLER_DEBUG(bool, g_packet_tx.car.lb);
    CONTROLLER_DEBUG(str, "\r\nRX: ");
    CONTROLLER_DEBUG(int, g_packet_tx.car.rx);
    CONTROLLER_DEBUG(str, "; RY: ");
    CONTROLLER_DEBUG(int, g_packet_tx.car.ry);
    CONTROLLER_DEBUG(str, "; RB: ");
    CONTROLLER_DEBUG(bool, g_packet_tx.car.rb);
    CONTROLLER_DEBUG(str, "\r\nPotentiometer: ");
    CONTROLLER_DEBUG(uint, g_packet_tx.car.pot);
    CONTROLLER_DEBUG(str, "\r\n--------\r\n");

    if (RADIO_write(g_packet_tx.buffer, PACKET_SIZE))
    {
        g_radio_status = _RADIO_DIS_TO_CON(g_radio_status);
        CONTROLLER_DEBUG(str, "send movement\r\n");
    }
    else
    {
        _CONTROLLER_disconnect();
    }
}

//------------------------------------------------------------------------------
// _CONTROLLER_status
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// _CONTROLLER_disconnect
//------------------------------------------------------------------------------
void _CONTROLLER_disconnect(void)
{
    CONTROLLER_DEBUG(str, "counter: ");
    CONTROLLER_DEBUG(int, _RADIO_COUNTER(g_radio_status));
    CONTROLLER_DEBUG(str, "\r\n");
    if (_RADIO_COUNTER(g_radio_status) < _RADIO_MAX_RETRY)
    {
        CONTROLLER_DEBUG(str, "################\r\n");
        ++g_radio_status;
    } // increase disconnection count
    else
    {
        g_radio_status = _RADIO_CON_TO_DIS(g_radio_status) | _RADIO_MAX_RETRY;
    } // disconnected and display
    CONTROLLER_DEBUG(str, "connection register: 0x");
    CONTROLLER_DEBUG(hex, g_radio_status, 2);
    CONTROLLER_DEBUG(str, "\r\n");
}

void _CONTROLLER_reset_connection(void)
{
    g_radio_status = 0;
    TFT_print_str(COL1, ROW_LAST, "searching...");
}
