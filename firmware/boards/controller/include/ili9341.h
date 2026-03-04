#ifndef VEMAR_ILI9341_H
#define VEMAR_ILI9341_H

#define DEFINE_TFT_ILI9341

#include "gpio.h"

typedef uint16_t ili9341_color_t;

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

void ILI9341_draw(uint16_t color);

#endif // VEMAR_ILI9341_H
