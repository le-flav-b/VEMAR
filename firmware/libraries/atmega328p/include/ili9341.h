#ifndef VEMAR_ILI9341_H
#define VEMAR_ILI9341_H

#define DEFINE_TFT_ILI9341

#include "gpio.h"
#include "color.h"

/**
 * @brief Define the orientation of the display
 */
typedef enum
{
    ILI9341_PORT_TR = 0x00, /**< Portrait, Right to Left, Top to Bottom*/
    ILI9341_LAND_BR = 0x20, /**< Landscape, Right to Left, Bottom to Top */
    ILI9341_PORT_TL = 0x40, /**< Portrait, Left to Right, Top to Bottom */
    ILI9341_LAND_TR = 0x60, /**< Landscape, Right to Left, Top to Bottom */
    ILI9341_PORT_BR = 0x80, /**< Portrait, Right to Left, Bottom to Top */
    ILI9341_LAND_BL = 0xA0, /**< Landscape, Left to Right, Bottom to Top */
    ILI9341_PORT_BL = 0xC0, /**< Portrait, Left to Right, Bottom to Top */
    ILI9341_LAND_TL = 0xE0  /**< Landscape, Left to Right, Top to Bottom */
} ili9341_orientation_t;

/**
 * @brief Initialize ILI9341
 * @param cs Chip Select pin
 * @param dc Data/Command pin
 * @param rst Reset pin
 */
void ILI9341_init(pin_t cs, pin_t dc, pin_t rst);

/**
 * @brief When the Software Reset command is written, it cause a software reset.
 * It resets the commands and parameters to their S/W Reset default values.
 * @note The Frame Memory contents are unaffected by this command
 */
void ILI9341_reset(void);

/**
 * @brief This commands is used to recover from __DISPLAY OFF__ mode.
 * Output from the Frame Memory is enabled.
 * This command makes no change of contents fo frame memory.
 * This command does not change any other status
 * @note This command has no effect when module is already in display on mode
 */
void ILI9341_enable_display(void);

/**
 * @brief This command is used to enter into __DISPLAY OFF__ mode. In this mode,
 * the output from Frame Memory is disabled and blank page is inserted.
 * This command makes no change of contents of frame memory.
 * This command does not change any other status.
 * There will be no abnormal visible effect on the display.
 * @note This command has no effect when module is already in display off mode
 */
void ILI9341_disable_display(void);

/**
 * @brief This command causes the LCD module to enter the minimum power
 * consumption mode.
 * In this mode e.g. the DC/DC converter is stopped, Internal oscillator
 * is stopped, and panel scanning is stopped
 * MCU interface and memory are still working and the memory keeps its contents
 */
void ILI9341_sleep_in(void);

/**
 * @brief This command turns off sleep mode.
 * In this mode e.g. the DC/DC converter is enabled, Internal oscillator
 * is started, and panel scanning is started
 * @note This command has no effect when module is already in Sleep Out mode.
 * Sleep Out Mode can only be left by the Sleep In command
 */
void ILI9341_sleep_out(void);

/**
 * @brief Set the orientation of the display
 * @param orientation Orientation of the display
 * @see ili9341_orientation_t
 */
void ILI9341_set_orientation(ili9341_orientation_t orientation);

/**
 * @brief Set the size of the text
 * @param size Size of the text
 */
void ILI9341_set_text_size(length_t size);

/**
 * @brief Set the spacing between characters
 * @param spacing Spacing
 */
void ILI9341_set_text_spacing(length_t spacing);

/**
 * @brief The the color of the text
 * @param color Color of the text
 */
void ILI9341_set_text_color(color16_t color);

/**
 * @brief The the background color of the text
 * @param background Background color of the text
 */
void ILI9341_set_text_background(color16_t background);

/**
 * @brief Fill the screen with the specified color
 * @param color Color to fill
 */
void ILI9341_fill_screen(color16_t color);

/**
 * @brief Fill the specified area with the color
 * @param x Starting position X of the area
 * @param y Starting position Y of the area
 * @param w Width of the area
 * @param h Height of the area
 * @param color Color to fill
 */
void ILI9341_fill_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color16_t color);

/**
 * @brief Display a single pixel
 * @param x Position X of the pixel
 * @param y Position Y of the pixel
 * @param color Color of the pixel
 */
void ILI9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Display a character
 * @param x Position Y of the character
 * @param y Position Y of the character
 * @param ch Character to display
 * @warning Only printable character
 */
void ILI9341_draw_char(uint16_t x, uint16_t y, char ch);

/**
 * @brief Display text on the screen
 * @param x Position X of the text
 * @param y Position Y of the text
 * @param str Text to display
 * @warning The text should only contain printable characters
 */
void ILI9341_draw_string(uint16_t x, uint16_t y, const char *str);

#endif // VEMAR_ILI9341_H
