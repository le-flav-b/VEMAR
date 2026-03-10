#ifndef VEMAR_TFT_H
#define VEMAR_TFT_H

#include "ili9341.h"
#include "color.h"

#if !defined(DEFINE_TFT_ILI9341)
#error Module 'ILI9341' not defined
#endif

// #define RGB565_WHITE 0xFFFF
// #define RGB565_YELLOW 0xFFE0
// #define RGB565_MAGENTA 0xF81F
// #define RGB565_RED 0xF800
// #define RGB565_CYAN 0x07FF
// #define RGB565_GREEN 0x07E0
// #define RGB565_BLUE 0x001F
// #define RGB565_BLACK 0x0000

/**
 * @brief Define the orientation of the display
 */
typedef enum
{
	TFT_PORTRAIT = 0,  /**< Portrait mode */
	TFT_LANDSCAPE = 1, /**< Landscape mode */
} tft_mode_t;

/**
 * @brief Define the direction how the pixels are displayed.
 * |            |     Normal    |    Inverted   |
 * |:----------:|:-------------:|:-------------:|
 * | Horizontal | Left to Right | Right to Left |
 * |  Vertical  | Top to Bottom | Bottom to Top |
 */
typedef enum
{
	TFT_INVERTED = 0, /**< Normal mode */
	TFT_NORMAL = 1	  /**< Inverted mode */

} tft_dir_t;

typedef enum
{
	TFT_TEXT_XS = 1,
	TFT_TEXT_S = 2,
	TFT_TEXT_M = 3,
	TFT_TEXT_L = 4,
	TFT_TEXT_XL = 5
} tft_text_t;

/**
 * @brief Initialize TFT display
 * @param cs Chip Select pin
 * @param dc Data/Command pin
 * @param rst Reset pin
 */
void TFT_init(pin_t cs, pin_t dc, pin_t rst);

/**
 * @brief Set the mode of the display
 * @param orientation Orientation of the display
 * @param x Horizontal direction
 * @param y Vertical direction
 */
void TFT_set_mode(tft_mode_t orientation, tft_dir_t x, tft_dir_t y);

void TFT_setup_text(tft_text_t size, length_t spacing, color16_t color, color16_t background);

/**
 * @brief Fill the screen with the specific color
 * @param color Color to fill the screen
 */
inline void TFT_fill_screen(color16_t color)
{
	ILI9341_fill_screen(color);
}

inline void TFT_print_char(uint16_t x, uint16_t y, char ch)
{
	ILI9341_draw_char(x, y, ch);
}

inline void TFT_print_str(uint16_t x, uint16_t y, const char *str)
{
	ILI9341_draw_string(x, y, str);
}


#endif // VEMAR_TFT_H
