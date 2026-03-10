#include "tft.h"

//------------------------------------------------------------------------------
// TFT_init
//------------------------------------------------------------------------------

void TFT_init(pin_t cs, pin_t dc, pin_t rst)
{
    ILI9341_init(cs, dc, rst);
    ILI9341_set_orientation(ILI9341_LAND_TL);
    ILI9341_set_text_size(TFT_TEXT_M);
    ILI9341_set_text_spacing(1);
    ILI9341_set_text_color(RGB16_BLACK);
    ILI9341_set_text_background(RGB16_WHITE);
}

//------------------------------------------------------------------------------
// TFT_set_mode
//------------------------------------------------------------------------------

void TFT_set_mode(tft_mode_t orientation, tft_dir_t x, tft_dir_t y)
{
    if (TFT_LANDSCAPE == orientation)
    {
        ILI9341_set_orientation((orientation << 1) | (y << 2) | (x << 3));
    }
    else
    {
        ILI9341_set_orientation((x << 2) | (y << 3));
    }
}

//------------------------------------------------------------------------------
// TFT_setup_text
//------------------------------------------------------------------------------

void TFT_setup_text(tft_text_t size,
                    length_t spacing,
                    color16_t color,
                    color16_t background)
{
    ILI9341_set_text_size(size);
    ILI9341_set_text_spacing(spacing);
    ILI9341_set_text_color(color);
    ILI9341_set_text_background(background);
}

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------

extern inline void TFT_fill_screen(color16_t color);
extern inline void TFT_print_char(uint16_t x, uint16_t y, char ch);
extern inline void TFT_print_str(uint16_t x, uint16_t y, const char *str);
