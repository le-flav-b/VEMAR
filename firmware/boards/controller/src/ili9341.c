#include "ili9341.h"
#include "spi.h"
#include "serial.h"

#define ILI9341_SPI_NOP 0x00       /**< No Operation */
#define ILI9341_SPI_SWRESET 0x01   /**< Software Reset */
#define ILI9341_SPI_RDDIFIF 0x04   /**< Read display identification information */
#define ILI9341_SPI_RDDST 0x09     /**< Read Display Status */
#define ILI9341_SPI_RDDPM 0x0A     /**< Read Display Power Mode */
#define ILI9341_SPI_RDDMADCTL 0x0B /**< Read Display Memory Access Control */
#define ILI9341_SPI_RDDCOLMOD 0x0C /**< Read Display Pixel Format */
#define ILI9341_SPI_RDDIM 0x0D     /**< Read Display Image Mode */
#define ILI9341_SPI_RDDSM 0x0E     /**< Read Display Signal Mode */
#define ILI9341_SPI_RDDSDR 0x0F    /**< Read Display Self-Diagnostic Result */
#define ILI9341_SPI_SPLIN 0x10     /**< Enter Sleep Mode */
#define ILI9341_SPI_SLPOUT 0x11    /**< Sleep out */
#define ILI9341_SPI_DISPOFF 0x28   /**< Display OFF */
#define ILI9341_SPI_DISPON 0x29    /**< Display ON */
#define ILI9341_SPI_CASET 0x2A     /**< Column Address Set */
#define ILI9341_SPI_PASET 0x2B     /**< Page Address Set */
#define ILI9341_SPI_RAMWR 0x2C     /**< Memory Write */
#define ILI9341_SPI_RGBSET 0x2D    /**< Color Set */
#define ILI9341_SPI_RAMRD 0x2E     /**< Memory Read */
#define ILI9341_SPI_PLTAR 0x30     /**< Partial Area */
#define ILI9341_SPI_VSCRDEF 0x33   /**< Vertical Scrolling Definition */
#define ILI9341_SPI_TEOFF 0x34     /**< Tearing Effect Line OFF */
#define ILI9341_SPI_TEON 0x35      /**< Tearing Effect Line ON */
#define ILI9341_SPI_MADCTL 0x36    /**< Memory Access Control */
#define ILI9341_SPI_VSCRSADD 0x37  /**< Vertical Scrolling Start Address */
#define ILI9341_SPI_IDMOFF 0x38    /**< Idle Mode OFF */
#define ILI9341_SPI_IDMON 0x39     /**< Idle Mode ON */
#define ILI9341_SPI_PIXSET 0x3A    /**< Pixel Format Set */

#define ILI9341_PIXEL_RGB16 0x50 /**< 16 bits / pixel RGB interface */
#define ILI9341_PIXEL_MCU16 0x05 /**< 16 bits / pixel MCU interface */

static pin_t g_ili9341_cs;  /**< LOW to start SPI communication */
static pin_t g_ili9341_dc;  /**< HIGH for data, LOW for command */
static pin_t g_ili9341_rst; /**< Reset */

static void ILI9341_set_command(byte_t cmd)
{
    PIN_mode(g_ili9341_dc, PIN_LOW);
    PIN_mode(g_ili9341_cs, PIN_LOW);
    SPI_transmit(cmd);
    PIN_mode(g_ili9341_cs, PIN_HIGH);
}

static void ILI9341_set_data(byte_t data)
{
    PIN_mode(g_ili9341_dc, PIN_HIGH);
    PIN_mode(g_ili9341_cs, PIN_LOW);
    SPI_transmit(data);
    PIN_mode(g_ili9341_cs, PIN_HIGH);
}

void ILI9341_sleep_in(void)
{
    ILI9341_set_command(ILI9341_SPI_SPLIN);
    delay(120);
}

void ILI9341_sleep_out(void)
{
    ILI9341_set_command(ILI9341_SPI_SLPOUT);
    delay(5);
}

void ILI9341_enable_display(void)
{
    ILI9341_set_command(ILI9341_SPI_DISPON);
}

void ILI9341_disable_display(void)
{
    ILI9341_set_command(ILI9341_SPI_DISPOFF);
}

/**
 * @brief This command is used to define area of frame memory where MCU can
 * access. This command makes no change on the other dirver status.
 * The values of __SC[15:0]__ and __EC[15:0]__ are referred when
 * __RAMWR__ command comes.
 * Each represents one column line in the Frame Memory
 * @param x Position X
 * @param y Position Y
 * @param w Width
 * @param h Height
 */
void ILI9341_define_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ILI9341_set_command(ILI9341_SPI_CASET);
    ILI9341_set_data((byte_t)(x >> 8));
    ILI9341_set_data((byte_t)(x & 0xFF));
    ILI9341_set_data((byte_t)((x + w) >> 8));
    ILI9341_set_data((byte_t)((x + w) & 0xFF));

    ILI9341_set_command(ILI9341_SPI_PASET);
    ILI9341_set_data((byte_t)(y >> 8));
    ILI9341_set_data((byte_t)(y & 0xFF));
    ILI9341_set_data((byte_t)((y + h) >> 8));
    ILI9341_set_data((byte_t)((y + h) & 0xFF));
}

void ILI9341_init(pin_t cs, pin_t dc, pin_t rst)
{
    g_ili9341_cs = cs;
    g_ili9341_dc = dc;
    g_ili9341_rst = rst;

    PIN_mode(g_ili9341_cs, PIN_OUTPUT);
    PIN_mode(g_ili9341_dc, PIN_OUTPUT);
    PIN_mode(g_ili9341_rst, PIN_OUTPUT);

    SPI_init(SPI_MSB, SPI_MODE0, SPI_PS4); // initialize SPI

    // reset hardware
    PIN_write(g_ili9341_rst, PIN_LOW);
    delay(20);
    PIN_write(g_ili9341_rst, PIN_HIGH);
    delay(150);

    ILI9341_reset();

    // set pixel format RGB565 (16 bits)
    ILI9341_set_command(ILI9341_SPI_PIXSET);
    ILI9341_set_data(ILI9341_PIXEL_RGB16 | ILI9341_PIXEL_MCU16);

    // turn on
    ILI9341_sleep_out();
    ILI9341_enable_display();
}

void ILI9341_reset(void)
{
    ILI9341_set_command(ILI9341_SPI_SWRESET);
    delay(120); // wait before sending new command
}

void ILI9341_draw(uint16_t color)
{
    ILI9341_set_command(ILI9341_SPI_RAMWR);
    ILI9341_set_data((byte_t)(color >> 8));
    ILI9341_set_data((byte_t)(color & 0xFF));
}
