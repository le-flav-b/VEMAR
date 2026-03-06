#include "tft.h"

void TFT_init(pin_t cs, pin_t dc, pin_t rst)
{
	ILI9341_init(cs, dc, rst);
	ILI9341_set_orientation(ILI9341_LAND_TL);
}

void TFT_set_orientation(tft_mode_t orientation, tft_mode_t horizontal, tft_mode_t vertical)
{
	if (TFT_LANDSCAPE == orientation) {
		ILI9341_set_orientation((orientation << 1) | (vertical << 2) | (horizontal << 3));
	} else {
		ILI9341_set_orientation((horizontal << 2) | (vertical << 3));
	}
}

extern inline void TFT_fill_screen(color16_t color);
