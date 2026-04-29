#include <avr/interrupt.h>

#include "controller.h"

#define _CONTROLLER_ADC_MAX 1023U

// Calibration of joystick
#define _ADC_CONVERT_LX(x) ((((int)x - 505) / 19) * 10)
#define _ADC_CONVERT_LY(x) ((((int)x - 518) / 20) * 10)
#define _ADC_CONVERT_RX(x) (((504 - (int)x) / 19) * 10)
#define _ADC_CONVERT_RY(x) (((538 - (int)x) / 20) * 10)

#define _CONTROLLER_MODE_COUNT 5
#define _CONTROLLER_MASK_MODE 0x0F           /**< Mask of display */
#define _CONTROLLER_MODE_ATM PACKET_ID_ATM   /**< Display atmospheric data */
#define _CONTROLLER_MODE_GAS PACKET_ID_GAS   /**< Display gas data */
#define _CONTROLLER_MODE_MAP PACKET_ID_LIDAR /**< Display map */
#define _CONTROLLER_MODE_GMC PACKET_ID_GMC   /**< Display radioactivity data */

#define _CONTROLLER_MODE_RX 0x10    /**< RX mode */
#define _CONTROLLER_MODE_TX 0x20    /**< TX mode */
#define _CONTROLLER_MASK_RADIO 0xF0 /**< Mask of Controller transmission mode */

#define _CONTROLLER_RX_DELAY 8000U /**< Delay for reception */

#define _DISPLAY_W 8U
#define _DISPLAY_H 8U
#define _DISPLAY_MODULE_OFFSET 12

/** @brief Maximum signal strength */
#define _RADIO_SIGNAL_MAX 0x10

controller_t g_controller;
packet_t g_packet;
packet_t g_packet_tx;

volatile byte_t g_ctrl_mode = 1;   /**< Controller mode flags */
volatile byte_t g_module_curr = 1; /**< Current display mode */
volatile byte_t g_module_en;       /**< Current enabled modules */
volatile byte_t g_radio_status;    /**< Radio strength */

/**
 * @brief Decrease signal strength count,
 * when it reaches to 0, the connection is lost
 */
static inline void _CONTROLLER_disconnect(void);

/**
 * @brief Increase signal strength count
 */
static inline void _CONTROLLER_connect(void);

/**
 * @brief Reset signal strength count
 */
static inline void _CONTROLLER_reset_connection(void);

/**
 * @brief Display layout
 */
static void _CONTROLLER_display_layout(const char *label);

/**
 * @brief Display available modules
 */
static void _CONTROLLER_display_module(void);

/**
 * @brief Check communication mode
 */
static void _CONTROLLER_set_radio_mode(void);

/**
 * @brief When multiple modules are connected, switch between them
 */
static void _CONTROLLER_switch_display(void);

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
    CONTROLLER_interrupt();

    // g_module_en = BIT(_CONTROLLER_MODE_MAP);
    CONTROLLER_display_menu();
    _CONTROLLER_reset_connection();
    _CONTROLLER_set_radio_mode();
    CONTROLLER_DEBUG(str, "end setup\r\n");
}

//------------------------------------------------------------------------------
// main loop
//------------------------------------------------------------------------------
void loop(void)
{
    if (BIT_is_set(g_ctrl_mode, _CONTROLLER_MODE_RX))
    {
        CONTROLLER_read();
    }
    if (BIT_is_set(g_ctrl_mode, _CONTROLLER_MODE_TX))
    {
        CONTROLLER_write();
    }
    CONTROLLER_update_connection();
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

void CONTROLLER_interrupt(void)
{
    BIT_set(PCICR, (BIT(PCIE1) | BIT(PCIE2)));      // enable interrupt (Port C & D)
    BIT_set(PCMSK1, (BIT(PCINT8) | BIT(PCINT9)));   // enable PC0 and PC1
    BIT_set(PCMSK2, (BIT(PCINT19) | BIT(PCINT20))); // enable PD3 and PD4
    sei();
}

//------------------------------------------------------------------------------
// CONTROLLER_display_data
//------------------------------------------------------------------------------
void CONTROLLER_display_atmosphere(void)
{
    if (_CONTROLLER_MODE_ATM != BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
    {
        TFT_fill_screen(RGB16_BLACK);
        _CONTROLLER_display_layout("MODULE: Atmosphere");
        TFT_print_str(COL1, ROW1, "Temperature: ");
        TFT_print_str(COL3, ROW1, "C");
        TFT_print_str(COL1, ROW2, "Humidity   : ");
        TFT_print_str(COL3, ROW2, "%");
        TFT_print_str(COL1, ROW3, "Pressure   : ");
        TFT_print_str(COL3, ROW3, "hPa");
        BIT_write(g_ctrl_mode, _CONTROLLER_MODE_ATM, _CONTROLLER_MASK_MODE);
    }
}

void CONTROLLER_display_gas(void)
{
    if (_CONTROLLER_MODE_GAS != BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
    {
        TFT_fill_screen(RGB16_BLACK);
        _CONTROLLER_display_layout("MODULE: Gas");
        TFT_print_str(COL1, ROW1, "CO2        : ");
        TFT_print_str(COL3, ROW1, "ppm");
        TFT_print_str(COL1, ROW2, "CO         : ");
        TFT_print_str(COL3, ROW2, "(raw ADC)");
        TFT_print_str(COL1, ROW3, "NH3        : ");
        TFT_print_str(COL3, ROW3, "(raw ADC)");
        TFT_print_str(COL1, ROW4, "NO2        : ");
        TFT_print_str(COL3, ROW4, "(raw ADC)");
        TFT_print_str(COL1, ROW5, "O2         : ");
        TFT_print_str(COL3, ROW5, "(raw ADC)");
        BIT_write(g_ctrl_mode, _CONTROLLER_MODE_GAS, _CONTROLLER_MASK_MODE);
    }
}

//------------------------------------------------------------------------------
// CONTROLLER_display_map
//------------------------------------------------------------------------------
void CONTROLLER_display_map(void)
{
    if (_CONTROLLER_MODE_MAP != BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
    {
        TFT_fill_screen(RGB16_BLACK);
        _CONTROLLER_display_layout("MODULE: LiDAR");
        BIT_write(g_ctrl_mode, _CONTROLLER_MODE_MAP, _CONTROLLER_MASK_MODE);
        // g_packet.lidar.line[0].row = 0;
        // g_packet.lidar.line[0].data[0] = 0xff;
        // g_packet.lidar.line[0].data[1] = 0x00;
        // g_packet.lidar.line[0].data[2] = 0xff;
        // g_packet.lidar.line[0].data[3] = 0x00;
        // g_packet.lidar.line[0].data[4] = 0xff;
        // CONTROLLER_update_map();
    }
}

void CONTROLLER_display_radioactivity(void)
{
    if (_CONTROLLER_MODE_GMC != BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
    {
        TFT_fill_screen(RGB16_BLACK);
        _CONTROLLER_display_layout("MODULE: Geiger Counter");
        TFT_print_str(COL1, ROW1, "Total      : ");
        TFT_print_str(COL1, ROW1, "Delta      : ");
        TFT_print_str(COL1, ROW1, "CPM        : ");
        BIT_write(g_ctrl_mode, _CONTROLLER_MODE_GMC, _CONTROLLER_MASK_MODE);
    }
}

void CONTROLLER_display_none(void)
{
    if (0 != g_module_en)
    {
        TFT_fill_screen(RGB16_BLACK);
        _CONTROLLER_display_layout("No Module detected");
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
    TFT_print_str(COL2, ROW1, str);

    str = UTIL_itoa(g_packet.gas.co, 4);
    TFT_print_str(COL2, ROW2, str);

    str = UTIL_itoa(g_packet.gas.nh3, 4);
    TFT_print_str(COL2, ROW3, str);

    str = UTIL_itoa(g_packet.gas.no2, 4);
    TFT_print_str(COL2, ROW4, str);

    str = UTIL_itoa(g_packet.gas.o2, 4);
    TFT_print_str(COL2, ROW5, str);
}

void CONTROLLER_update_map(void)
{
#define LINE_SIZE 8
#define LINE_OFFSET 64

    for (uint16_t row = 0; row < LIDAR_DATA_PER_PACKET; ++row)
    {
        for (uint16_t col = 0; col < LIDAR_DATA_PER_LINE; ++col)
        {
            for (length_t pos = 0; pos < LINE_SIZE; ++pos)
            {
                if (BIT_read((g_packet.lidar.line[row]).data[col], BIT(pos)))
                {
                    uint16_t x = (LINE_SIZE - 1 - pos) * _DISPLAY_W + LINE_OFFSET * col;
                    uint16_t y = ((g_packet.lidar.line[row]).row * _DISPLAY_H) + ROW1;
                    TFT_fill_area(x, y, _DISPLAY_W, _DISPLAY_H, RGB16_WHITE);
                }
            }
        }
    }
}

void CONTROLLER_update_radioactivity(void)
{
    char *str;

    str = UTIL_itoa(g_packet.geiger.total, 6);
    TFT_print_str(COL2, ROW1, str);

    str = UTIL_itoa(g_packet.geiger.delta, 6);
    TFT_print_str(COL2, ROW2, str);

    str = UTIL_itoa(g_packet.geiger.cpm, 6);
    TFT_print_str(COL2, ROW3, str);
}

//------------------------------------------------------------------------------
// CONTROLLER_update_connection
//------------------------------------------------------------------------------
void CONTROLLER_update_connection(void)
{
    char *signal = UTIL_itoa((g_radio_status >> 2), 2);
    TFT_print_str(86, ROW_LAST, signal);
}

void _CONTROLLER_handle_packet(byte_t type, void (*callback)(void))
{
    BIT_set(g_module_en, BIT(type));
    if (BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE) == type)
    {
        callback();
    }
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
        _CONTROLLER_connect();
        CONTROLLER_DEBUG(str, "packet ID: ");
        CONTROLLER_DEBUG(int, g_packet.header.id);
        CONTROLLER_DEBUG(str, " received\r\n");

        switch (g_packet.header.id)
        {
        case PACKET_ID_CAR:
            g_module_en = g_packet.header.module;
            _CONTROLLER_display_module();
            break;
        case PACKET_ID_ATM:
            _CONTROLLER_handle_packet(_CONTROLLER_MODE_ATM,
                                      CONTROLLER_update_atmosphere);
            break;
        case PACKET_ID_GAS:
            _CONTROLLER_handle_packet(_CONTROLLER_MODE_GAS,
                                      CONTROLLER_update_gas);
            break;
        case PACKET_ID_LIDAR:
            _CONTROLLER_handle_packet(_CONTROLLER_MODE_MAP,
                                      CONTROLLER_update_map);
            break;
        case PACKET_ID_GMC:
            _CONTROLLER_handle_packet(_CONTROLLER_MODE_GMC,
                                      CONTROLLER_update_radioactivity);
            break;
        default:
            break;
        }

        // if (PACKET_ID_CAR == g_packet.header.id)
        // {
        //     g_module_en = g_packet.header.module;
        //     _CONTROLLER_display_module();
        // }
        // else if (PACKET_ID_ATM == g_packet.header.id)
        // {
        //     BIT_set(g_module_en, BIT(_CONTROLLER_MODE_ATM));
        //     if (_CONTROLLER_MODE_ATM == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
        //     {
        //         CONTROLLER_update_atmosphere();
        //     }
        // }
        // else if (PACKET_ID_GAS == g_packet.header.id)
        // {
        //     BIT_set(g_module_en, BIT(_CONTROLLER_MODE_GAS));
        //     if (_CONTROLLER_MODE_GAS == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
        //     {
        //         CONTROLLER_update_gas();
        //     }
        // }
        // else if (PACKET_ID_GMC == g_packet.header.id)
        // {
        //     BIT_set(g_module_en, BIT(_CONTROLLER_MODE_GMC));
        //     if (_CONTROLLER_MODE_GMC == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
        //     {
        //         CONTROLLER_update_radioactivity();
        //     }
        // }
        // else if (PACKET_ID_LIDAR == g_packet.header.id)
        // {
        //     BIT_set(g_module_en, BIT(_CONTROLLER_MODE_MAP));
        //     if (_CONTROLLER_MODE_MAP == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_MODE))
        //     {
        //         CONTROLLER_display_map();
        //     }
        // }
        // else
        // {
        //     CONTROLLER_DEBUG(str, "unknown package ID: ");
        //     CONTROLLER_DEBUG(int, g_packet.header.id);
        //     CONTROLLER_DEBUG(str, "\r\n");
        // }
        // _CONTROLLER_connect();
        // CONTROLLER_DEBUG(str, "packet received\r\n");
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

    for (length_t attempt = 0; attempt < 5; ++attempt)
    {
        if (RADIO_write(g_packet_tx.buffer, PACKET_SIZE))
        {
            _CONTROLLER_connect();
            CONTROLLER_DEBUG(str, "send movement\r\n");
            return;
        }
    }
    _CONTROLLER_disconnect();
    CONTROLLER_DEBUG(str, "transmission failed\r\n");

    // if (RADIO_write(g_packet_tx.buffer, PACKET_SIZE))
    // {
    //     _CONTROLLER_connect();
    //     CONTROLLER_DEBUG(str, "send movement\r\n");
    // }
    // else
    // {
    //     _CONTROLLER_disconnect();
    //     CONTROLLER_DEBUG(str, "transmission failed\r\n");
    // }
}

//------------------------------------------------------------------------------
// _CONTROLLER_connect
//------------------------------------------------------------------------------
void _CONTROLLER_connect(void)
{
    if (_RADIO_SIGNAL_MAX > g_radio_status)
    {
        ++g_radio_status;
    }
}

//------------------------------------------------------------------------------
// _CONTROLLER_disconnect
//------------------------------------------------------------------------------
void _CONTROLLER_disconnect(void)
{
    if (0 < g_radio_status)
    {
        --g_radio_status;
    }
}

//------------------------------------------------------------------------------
// _CONTROLLER_reset_disconnect
//------------------------------------------------------------------------------
void _CONTROLLER_reset_connection(void)
{
    g_radio_status = 8;
}

//------------------------------------------------------------------------------
// _CONTROLLER_display_menu
//------------------------------------------------------------------------------
void CONTROLLER_display_menu(void)
{
    if (0 != g_ctrl_mode)
    {
        TFT_fill_screen(RGB16_BLACK);
        TFT_setup_text(TFT_TEXT_XXL, 8, RGB16_WHITE, RGB16_BLACK);
        TFT_print_str(60, 100, "VEMAR");
        TFT_setup_text(TFT_TEXT_S, 1, RGB16_WHITE, RGB16_BLACK);
        TFT_print_str(COL1, ROW_LAST, "signal: ");
        _CONTROLLER_display_module();
        g_ctrl_mode = 0;
    }
}

//------------------------------------------------------------------------------
// _CONTROLLER_display_module_intern
//------------------------------------------------------------------------------
void _CONTROLLER_display_module_intern(byte_t mod, uint16_t offset, char label)
{
    if (BIT_is_set(g_module_en, BIT(mod)))
    {
        if (mod != g_module_curr)
        {
            TFT_setup_text(TFT_TEXT_S, 1, RGB16_GRAY, RGB16_BLACK);
            TFT_print_char(COL_MOD + _DISPLAY_MODULE_OFFSET * offset, ROW_LABEL, label);
            TFT_setup_text(TFT_TEXT_S, 1, RGB16_WHITE, RGB16_BLACK);
        }
        else
        {
            TFT_print_char(COL_MOD + _DISPLAY_MODULE_OFFSET * offset, ROW_LABEL, label);
        }
    }
    else
    {
        TFT_print_char(COL_MOD + _DISPLAY_MODULE_OFFSET * offset, ROW_LABEL, ' ');
    }
}

//------------------------------------------------------------------------------
// _CONTROLLER_display_module
//------------------------------------------------------------------------------
void _CONTROLLER_display_module(void)
{
    _CONTROLLER_display_module_intern(_CONTROLLER_MODE_ATM, 0, '1');
    _CONTROLLER_display_module_intern(_CONTROLLER_MODE_GAS, 1, '2');
    _CONTROLLER_display_module_intern(_CONTROLLER_MODE_MAP, 2, '3');
    _CONTROLLER_display_module_intern(_CONTROLLER_MODE_GMC, 3, '4');
}

//------------------------------------------------------------------------------
// _CONTROLLER_display_layout
//------------------------------------------------------------------------------
void _CONTROLLER_display_layout(const char *label)
{
    TFT_print_str(COL1, ROW_LABEL, label);
    TFT_fill_area(0, ROW_LABEL + 14, TFT_HEIGHT, 2, RGB16_WHITE);

    _CONTROLLER_display_module();

    TFT_fill_area(0, ROW_LAST - 4, TFT_HEIGHT, 2, RGB16_WHITE);
    TFT_print_str(COL1, ROW_LAST, "signal: ");

    if (_CONTROLLER_MODE_RX == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_RADIO))
    {
        TFT_print_str(COL_RXTX, ROW_LAST, "Rx   ");
    }
    else if (_CONTROLLER_MODE_TX == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_RADIO))
    {
        TFT_print_str(COL_RXTX, ROW_LAST, "   Tx");
    }
    else
    {
        TFT_print_str(COL_RXTX, ROW_LAST, "Rx/Tx");
    }
}

//------------------------------------------------------------------------------
// _CONTROLLER_set_radio_mode
//------------------------------------------------------------------------------
void _CONTROLLER_set_radio_mode(void)
{
    BIT_set(g_ctrl_mode, (_CONTROLLER_MODE_RX | _CONTROLLER_MODE_TX));
    if (BIT_is_clear(PIND, BIT(PIND3)))
    {
        BIT_clear(g_ctrl_mode, _CONTROLLER_MODE_RX);
    } // if TX enabled
    if (BIT_is_clear(PIND, BIT(PIND4)))
    {
        BIT_clear(g_ctrl_mode, _CONTROLLER_MODE_TX);
    } // if RX enabled

    if (_CONTROLLER_MODE_RX == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_RADIO))
    {
        TFT_print_str(COL_RXTX, ROW_LAST, "Rx   ");
    }
    else if (_CONTROLLER_MODE_TX == BIT_read(g_ctrl_mode, _CONTROLLER_MASK_RADIO))
    {
        TFT_print_str(COL_RXTX, ROW_LAST, "   Tx");
    }
    else
    {
        TFT_print_str(COL_RXTX, ROW_LAST, "Rx/Tx");
    }
}

//------------------------------------------------------------------------------
// _CONTROLLER_switch_display
//------------------------------------------------------------------------------
void _CONTROLLER_switch_display(void)
{
    for (length_t i = 0; i < _CONTROLLER_MODE_COUNT; ++i)
    {
        if (++g_module_curr > _CONTROLLER_MODE_COUNT)
        {
            g_module_curr = _CONTROLLER_MODE_ATM;
        }

        if (BIT_is_clear(g_module_en, BIT(g_module_curr)))
        {
            continue;
        }

        switch (g_module_curr)
        {
        case _CONTROLLER_MODE_ATM:
            CONTROLLER_display_atmosphere();
            return;
        case _CONTROLLER_MODE_GAS:
            CONTROLLER_display_gas();

            return;
        case _CONTROLLER_MODE_MAP:
            CONTROLLER_display_map();

            return;
        case _CONTROLLER_MODE_GMC:
            CONTROLLER_display_radioactivity();

            return;
        default:
            return;
        }
    }
    CONTROLLER_display_none();
}

//------------------------------------------------------------------------------
// Interruption on Button A & Button B
//------------------------------------------------------------------------------
ISR(PCINT1_vect)
{
    if (BIT_is_clear(PINC, BIT(PINC0)))
    {
        _CONTROLLER_switch_display();
    } // PC0 interrupt

    if (BIT_is_clear(PINC, BIT(PINC1)))
    {
        /// @todo Function assigned to button B
    } // PC1 interrupt
}

//------------------------------------------------------------------------------
// Interruption on Toggle
//------------------------------------------------------------------------------
ISR(PCINT2_vect)
{
    _CONTROLLER_set_radio_mode();
}
