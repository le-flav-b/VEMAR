#include "tft.h"
#include "serial.h"

#define PIN_CS PIN_PB2
#define PIN_DC PIN_PB0
#define PIN_RST PIN_PB1

void setup(void)
{
	SERIAL_init();
	TFT_init(PIN_CS, PIN_DC, PIN_RST);
	SERIAL_println(str, "setup");

	// ILI9341_set_orientation(ILI9341_PORT_TL);
	// TFT_set_mode(TFT_PORTRAIT, TFT_INVERTED, TFT_NORMAL);
	TFT_setup_text(TFT_TEXT_S, 1, RGB16_BLUE, RGB16_BLACK);
	TFT_fill_screen(RGB16_YELLOW);
	// ILI9341_draw_char(0, 0, 'P');
	// ILI9341_draw_char(32, 0, 'A');
	// ILI9341_draw_char(64, 0, 'R');
	// ILI9341_draw_char(96, 0, 'I');
	// ILI9341_draw_char(128, 0, 'S');
	TFT_print_str(0, 0, "abcdefg");
	TFT_print_str(0, 32, "hijklmnop");
	TFT_print_str(0, 64, "qrstuvwxyz");

	// ILI9341_draw_char(0, 0, 'P', RGB565_RED, RGB565_BLACK);
	// ILI9341_draw_char(32, 0, 'A', RGB565_RED, RGB565_BLACK);
	// ILI9341_draw_char(64, 0, 'R', RGB565_RED, RGB565_BLACK);
	// ILI9341_draw_char(96, 0, 'I', RGB565_RED, RGB565_BLACK);
	// ILI9341_draw_char(128, 0, 'S', RGB565_RED, RGB565_BLACK);
}

void loop(void)
{

}
