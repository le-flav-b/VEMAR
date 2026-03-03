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
 * @brief Initialize TFT display
 * @param cs Chip Select pin
 * @param dc Data/Command pin
 * @param rst Reset pin
 */
void TFT_init(pin_t cs, pin_t dc, pin_t rst);



#endif // VEMAR_TFT_H
