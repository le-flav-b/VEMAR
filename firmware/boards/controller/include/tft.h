#ifndef VEMAR_TFT_H
#define VEMAR_TFT_H

#include "ili9341.h"

#if !defined(DEFINE_TFT_ILI9341)
#error Module 'ILI9341' not defined
#endif

#define RGB565_WHITE 0xFFFF
#define RGB565_YELLOW 0xFFE0
#define RGB565_MAGENTA 0xF81F
#define RGB565_RED 0xF800
#define RGB565_CYAN 0x07FF
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F
#define RGB565_BLACK 0x0000

/**
 * @brief Define the orientation of the display
 * @details
 * - In __Portrait__ mode
 * |            |  Normal  | Inverted |
 * | :--------: | :------: | :------: |
 * | Horizontal |  R -> L  |  L -> R  |
 * |  Vertical  |  T -> B  |  B -> T  |
 *
 * - In __Landscape__ mode
 * |            |  Normal  | Inverted |
 * | :--------: | :------: | :------: |
 * | Horizontal |  R -> L  |  L -> R  |
 * |  Vertical  |  T -> B  |  B -> T  |
 */
typedef enum
{
	TFT_PORTRAIT = 0,  /**< Portrait mode */
	TFT_LANDSCAPE = 1, /**< Landscape mode */
	TFT_INVERTED = 0,  /**< Horizontal: Right to Left; Vertical: Bottom to Top */
	TFT_NORMAL = 1	   /**< Horizontal: Left to Right; Vertical: Top to Bottom */
} tft_mode_t;

/**
 * @brief Initialize TFT display
 * @param cs Chip Select pin
 * @param dc Data/Command pin
 * @param rst Reset pin
 */
void TFT_init(pin_t cs, pin_t dc, pin_t rst);

void TFT_set_mode(tft_mode_t orientation, tft_mode_t horizontal, tft_mode_t vertical);

/**
 * @brief Fill the screen with the specific color
 * @param color Color to fill the screen
 */
inline void TFT_fill_screen(color16_t color)
{
	ILI9341_fill_screen(color);
}

#endif // VEMAR_TFT_H
