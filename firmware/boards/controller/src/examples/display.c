#include "tft.h"
#include "serial.h"
#include "spi.h"

led_t led;

void setup(void)
{
	SERIAL_init();
	TFT_init(PIN_PB2, PIN_PB0, PIN_PB1);
	SERIAL_println(str, "setup");

	TFT_fill_screen(RGB565_YELLOW);
	ILI9341_draw_char(0, 0, 'P', RGB565_RED, RGB565_BLACK);
	ILI9341_draw_char(32, 0, 'A', RGB565_RED, RGB565_BLACK);
	ILI9341_draw_char(64, 0, 'R', RGB565_RED, RGB565_BLACK);
	ILI9341_draw_char(96, 0, 'I', RGB565_RED, RGB565_BLACK);
	ILI9341_draw_char(128, 0, 'S', RGB565_RED, RGB565_BLACK);
}

void loop(void)
{

}
