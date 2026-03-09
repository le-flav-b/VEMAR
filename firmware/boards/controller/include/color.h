#ifndef VEMAR_COLOR_H
#define VEMAR_COLOR_H

/**
 * @brief Convert 24-bit RGB to 16-bit RGB
 * @param r 8-bit Red
 * @param g 8-bit Green
 * @param b 8-bit Blue
 * @note This is a slower than COLOR_FAST_RGB16, but more accurate
 * @see COLOR_FAST_RGB16
 */
#define COLOR_RGB16(r, g, b)      \
    ((((r) * 31U / 255U) << 11) | \
     (((g) * 63U / 255U) << 5) |  \
     (((b) * 31U / 255U)))

/**
 * @brief Convert 24-bit RGB to 16-bit RGB
 * @param r 8-bit Red
 * @param g 8-bit Green
 * @param b 8-bit Blue
 * @note This is faster than COLOR_RGB16, but less accurate
 * @see COLOR_RGB16
 */
#define COLOR_FAST_RGB16(r, g, b) \
    ((((r) & 0xF8) << 8) |        \
     (((g) & 0xFC) << 3) |        \
     (((b) & 0xF8) >> 3))

/**
 * @brief Common 16-bit color code
 * @{
 */
#define RGB16_WHITE 0xFFFF   /**< White */
#define RGB16_YELLOW 0xFFE0  /**< Yellow */
#define RGB16_MAGENTA 0xF81F /**< Magenta */
#define RGB16_RED 0xF800     /**< Red */
#define RGB16_CYAN 0x07FF    /**< Cyan */
#define RGB16_GREEN 0x07E0   /**< Green */
#define RGB16_BLUE 0x001F    /**< Blue */
#define RGB16_BLACK 0x0000   /**< Black */
/**
 * @}
 */

/**
 * @brief Define 16-bit RGB color
 */
typedef uint16_t color16_t;

#endif // VEMAR_COLOR_H
