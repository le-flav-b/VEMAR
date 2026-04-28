#ifndef VEMAR_TFT_H
#define VEMAR_TFT_H

#include "ili9341.h"

#if !defined(DEFINE_TFT_ILI9341)
#error Module 'ILI9341' not defined
#endif

#define TFT_WIDTH 240
#define TFT_HEIGHT 320

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

/**
 * @brief Define text size
 */
typedef enum
{
	TFT_TEXT_XS = 1, /**< Extra small (5x7) */
	TFT_TEXT_S = 2,	 /**< Small (10x14) */
	TFT_TEXT_M = 3,	 /**< Medium (15x21) */
	TFT_TEXT_L = 4,	 /**< Large (20x28) */
	TFT_TEXT_XL = 5, /**< Extra Large (25x35) */
	TFT_TEXT_XXL = 6 /**< Double Extra Large (30x42) */
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

/**
 * @brief Modify text configuration
 * @param size Size of the text
 * @param spacing Spacing between characters
 * @param color Color of the text
 * @param background Text background
 * @see tft_text_t
 */
void TFT_setup_text(tft_text_t size,
					length_t spacing,
					color16_t color,
					color16_t background);

/**
 * @brief Fill the screen with the specific color
 * @param color Color to fill the screen with
 */
inline void TFT_fill_screen(color16_t color)
{
	ILI9341_fill_screen(color);
}

/**
 * @brief Fill the defined area defined with the specific color
 * @param x Position on X-axis
 * @param y Position on Y-axis
 * @param w Width
 * @param h Height
 * @param color Color
 */
inline void TFT_fill_area(uint16_t x, uint16_t y,
						  uint16_t w, uint16_t h,
						  color16_t color)
{
	ILI9341_fill_area(x, y, w, h, color);
}

/**
 * @brief Display a character
 * @param x Position X of the character (Top-Left)
 * @param y Position Y of the character (Top-Left)
 * @param ch Character to display
 */
inline void TFT_print_char(uint16_t x, uint16_t y, char ch)
{
	ILI9341_draw_char(x, y, ch);
}

/**
 * @brief Display a text
 * @param x Position X of the text (Top-Left)
 * @param y Position Y of the text (Top-Left)
 * @param str Null-terminated string
 */
inline void TFT_print_str(uint16_t x, uint16_t y, const char *str)
{
	ILI9341_draw_string(x, y, str);
}

#endif // VEMAR_TFT_H
