#include "tft.h"
#include "serial.h"
#include "radio.h"
#include "gpio.h"
#include "util.h"

#define VALUE_MAX (10)
#define VALUE_MIN (-10)

button_t btn_up;
button_t btn_down;
led_t led;
analog_t potentiometer;

typedef union
{
    uint16_t value;
    byte_t buffer[32];
} packet_t;

typedef struct
{
    button_t up;
    button_t down;
} toggle_t;

packet_t payload;
toggle_t toggle;

static int value_old = -1;
static int value = 0;

#define POS_X 172

static inline void display_temperature(int value)
{
    char *str = UTIL_itoa_decimal(value, 4);
    TFT_print_str(128, 4, str);
}

static inline void display_humidity(int value)
{
    char *str = UTIL_itoa_decimal(value, 4);
    TFT_print_str(128, 20, str);
}

void setup_tft(void)
{
    TFT_init(PIN_PB2, PIN_PB0, PIN_PB1);
    TFT_setup_text(TFT_TEXT_S, 0, RGB16_WHITE, RGB16_BLACK);
    TFT_fill_screen(RGB16_BLACK);
    TFT_print_str(4, 4, "Temperature: ");
    TFT_print_str(POS_X, 4, "C");
    TFT_print_str(4, 20, "Humidity   : ");
    TFT_print_str(POS_X, 20, "%");
}

void setup(void)
{
    SERIAL_init();
    RADIO_init(PIN_PD6, PIN_PD7);
    potentiometer = ANALOG_new(ADC_CH4);
    led = LED_new(PIN_PD5);
    btn_up = BUTTON_new(PIN_PD4, BUTTON_ONPRESS);
    btn_down = BUTTON_new(PIN_PD3, BUTTON_ONPRESS);
    toggle.up = BUTTON_new(PIN_PC3, BUTTON_ONHOLD);
    toggle.down = BUTTON_new(PIN_PC2, BUTTON_ONHOLD);
    setup_tft();
    LED_on(led);
}

void loop(void)
{
    if (BUTTON_is_active(&btn_up))
    {
        if (++value > VALUE_MAX)
        {
            value = VALUE_MAX;
        }
    }
    if (BUTTON_is_active(&btn_down))
    {
        if (--value < VALUE_MIN)
        {
            value = VALUE_MIN;
        }
    }
    if (value != value_old)
    {
        value_old = value;
        display_temperature(value);
        display_humidity(value + 42);
    }

    payload.value = ANALOG_read(potentiometer);
    if (BUTTON_is_active(&(toggle.up)))
    {
        RADIO_write(payload.buffer, 32);
    }
}
