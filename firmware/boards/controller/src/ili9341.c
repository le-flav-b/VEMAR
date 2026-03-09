#include "ili9341.h"
#include "spi.h"
#include "font.h"

#include "serial.h"

#define ILI9341_WIDTH 240  /**< Screen width */
#define ILI9341_HEIGHT 320 /**< Screen height */

#define ILI9341_UTIL_SIZE(w, h) ((uint32_t)w * (uint32_t)h)

#define ILI9341_SPI_NOP 0x00       /**< No Operation */
#define ILI9341_SPI_SWRESET 0x01   /**< Software Reset */
#define ILI9341_SPI_RDDIDIF 0x04   /**< Read display identification information */
#define ILI9341_SPI_RDDST 0x09     /**< Read Display Status */
#define ILI9341_SPI_RDDPM 0x0A     /**< Read Display Power Mode */
#define ILI9341_SPI_RDDMADCTL 0x0B /**< Read Display Memory Access Control */
#define ILI9341_SPI_RDDCOLMOD 0x0C /**< Read Display Pixel Format */
#define ILI9341_SPI_RDDIM 0x0D     /**< Read Display Image Mode */
#define ILI9341_SPI_RDDSM 0x0E     /**< Read Display Signal Mode */
#define ILI9341_SPI_RDDSDR 0x0F    /**< Read Display Self-Diagnostic Result */
#define ILI9341_SPI_SPLIN 0x10     /**< Enter Sleep Mode */
#define ILI9341_SPI_SLPOUT 0x11    /**< Sleep out */
#define ILI9341_SPI_PTLON 0x12     /**< Partial Mode On */
#define ILI9341_SPI_NORON 0x13     /**< Normal Display Mode On */
#define ILI9341_SPI_DINVOFF 0x20   /**< Display Inversion OFF */
#define ILI9341_SPI_DINON 0x21     /**< Display Inversion ON */
#define ILI9341_SPI_GAMSET 0x26    /**< Gamma Set */
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
#define ILI9341_SPI_WRDISBV 0x51   /**< Write Display Brightness */
#define ILI9341_SPI_RDDISBV 0x52   /**< Read Display Brightness Value */
#define ILI9341_SPI_WRCTRLD 0x53   /**< Write Control Display */
#define ILI9341_SPI_RDCTRLD 0x54   /**< Read Control Display */
#define ILI9341_SPI_WRCABC 0x55    /**< Wrtie Content Adaptive Brightness Control */
#define ILI9341_SPI_RDCABC 0x56    /**< Read Content Adaptive Brightness Control */
#define ILI9341_SPI_RDID1 0xDA     /**< Read ID 1 */
#define ILI9341_SPI_RDID2 0xDB     /**< Read ID 2 */
#define ILI9341_SPI_RDID3 0xDC     /**< Read ID 3 */
#define ILI9341_SPI_IFMODE 0xB0    /**< Interface Mode Control */
#define ILI9341_SPI_FRMCTR1 0xB1   /**< Frame Rate Control (Normal mode/Full colors) */
#define ILI9341_SPI_FRMCTR2 0xB2   /**< Frame Rate Control (Idle Mode/8I colors) */
#define ILI9341_SPI_FRMCTR3 0xB3   /**< Frame Rate Control (Partial Mode/Full colors) */
#define ILI9341_SPI_INVTR 0xB4     /**< Display Inversion Control */
#define ILI9341_SPI_PRCTR 0xB5     /**< Blanking Porch */
#define ILI9341_SPI_DISCTRL 0xB6   /**< Display Function Control */
#define ILI9341_SPI_ETMOD 0xB7     /**< Entry Mode Set */
#define ILI9341_SPI_PWCTRL1 0xC0   /**< Power Control 1 */
#define ILI9341_SPI_PWCTRL2 0xC1   /**< Power Control 2 */
#define ILI9341_SPI_VMCTRL1 0xC5   /**< VCOM Control 1 */
#define ILI9341_SPI_VMCTRL2 0xC7   /**< VCOM Control 2 */
#define ILI9341_SPI_NVMWR 0xD0     /**< NV Memory Write */
#define ILI9341_SPI_NVMPKEY 0xD1   /**< NV Memory Protection Key */
#define ILI9341_SPI_RDNVM 0xD2     /**< NV Memory Status Read */
#define ILI9341_SPI_RDID4 0xD3     /**< Read ID 4 */
#define ILI9341_SPI_PGAMCTRL 0xE0  /**< Positive Gamma Control */
#define ILI9341_SPI_NGAMCTRL 0xE1  /**< Negative Gamma Correction */
#define ILI9341_SPI_DGAMCTRL1 0xE2 /**< Digital Gamma Control 1 */
#define ILI9341_SPI_DGAMCTRL2 0xE3 /**< Digital Gamma Control 2 */
#define ILI9341_SPI_IFCTL 0xF6     /**< 16bits Data Format Selection */

#define ILI9341_MADCTL_BGR 0x08 /**< BIT 3 of MADCTL */
#define ILI9341_MADCTL_MV 0x20  /**< BIT 5 of MADCTL */

#define ILI9341_RGB16 0x55 /**< 16-bit RGB */

// static pin_t g_ili9341_cs;  /**< LOW to start SPI communication */
// static pin_t g_ili9341_dc;  /**< HIGH for data, LOW for command */
// static pin_t g_ili9341_rst; /**< Reset */

#define ILI9341_MASK_ORIENTATION 0xE0 /**< MADCTL[7:5] */

typedef struct
{
    length_t size;        /**< Size of the text */
    length_t spacing;     /**< Space betwwen characters in pixels */
    color16_t color;      /**< Color of the text */
    color16_t background; /**< Color of the background */
} ili9341_text_t;

struct ili9341_t
{
    uint16_t w;          /**< Width */
    uint16_t h;          /**< Height */
    pin_t cs;            /**< Chip Select (LOW to start SPI communication) */
    pin_t dc;            /**< Data/Command (HIGH for data, LOw for command) */
    pin_t rst;           /**< Reset */
    byte_t madctl;       /**< MADCTL register */
    ili9341_text_t text; /**< Text configuration */
};

static struct ili9341_t g_ili9341;

//------------------------------------------------------------------------------
// ILI9341_set_command
//------------------------------------------------------------------------------

static void ILI9341_set_command(byte_t cmd)
{
    PIN_write(g_ili9341.dc, PIN_LOW);
    PIN_write(g_ili9341.cs, PIN_LOW);
    SPI_transmit(cmd);
    PIN_write(g_ili9341.cs, PIN_HIGH);
}

//------------------------------------------------------------------------------
// ILI9341_set_data
//------------------------------------------------------------------------------

static void ILI9341_set_data(byte_t data)
{
    PIN_write(g_ili9341.dc, PIN_HIGH);
    PIN_write(g_ili9341.cs, PIN_LOW);
    SPI_transmit(data);
    PIN_write(g_ili9341.cs, PIN_HIGH);
}

//------------------------------------------------------------------------------
// ILI9341_set_data16
//------------------------------------------------------------------------------

static void ILI9341_set_data16(uint16_t data)
{
    PIN_write(g_ili9341.dc, PIN_HIGH);
    PIN_write(g_ili9341.cs, PIN_LOW);
    SPI_transmit((byte_t)(data >> 8));
    SPI_transmit((byte_t)(data & 0xFF));
    PIN_write(g_ili9341.cs, PIN_HIGH);
}

static void ILI9341_setup_rgb(void)
{
    BIT_set(g_ili9341.madctl, ILI9341_MADCTL_BGR); // invert BGR <-> RGB
    ILI9341_set_command(ILI9341_SPI_MADCTL);
    ILI9341_set_data(g_ili9341.madctl);
    // set pixel format RGB565 (16 bits)
    ILI9341_set_command(ILI9341_SPI_PIXSET);
    ILI9341_set_data(ILI9341_RGB16);
}

//------------------------------------------------------------------------------
// ILI9341_sleep_in
//------------------------------------------------------------------------------

void ILI9341_sleep_in(void)
{
    ILI9341_set_command(ILI9341_SPI_SPLIN);
    delay(120);
}

//------------------------------------------------------------------------------
// ILI9341_sleep_out
//------------------------------------------------------------------------------

void ILI9341_sleep_out(void)
{
    ILI9341_set_command(ILI9341_SPI_SLPOUT);
    delay(5);
}

//------------------------------------------------------------------------------
// ILI9341_enable_display
//------------------------------------------------------------------------------

void ILI9341_enable_display(void)
{
    ILI9341_set_command(ILI9341_SPI_DISPON);
}

//------------------------------------------------------------------------------
// ILI9341_disable_display
//------------------------------------------------------------------------------

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
    ILI9341_set_data16(x);
    ILI9341_set_data16(x + w - 1);

    ILI9341_set_command(ILI9341_SPI_PASET);
    ILI9341_set_data16(y);
    ILI9341_set_data16(y + h - 1);

    ILI9341_set_command(ILI9341_SPI_RAMWR);
}

//------------------------------------------------------------------------------
// ILI9341_init
//------------------------------------------------------------------------------

void ILI9341_init(pin_t cs, pin_t dc, pin_t rst)
{
    g_ili9341.cs = cs;
    g_ili9341.dc = dc;
    g_ili9341.rst = rst;
    g_ili9341.w = ILI9341_WIDTH;
    g_ili9341.h = ILI9341_HEIGHT;

    PIN_mode(g_ili9341.cs, PIN_OUTPUT);
    PIN_mode(g_ili9341.dc, PIN_OUTPUT);
    PIN_mode(g_ili9341.rst, PIN_OUTPUT);

    SPI_init(SPI_MSB, SPI_MODE0, SPI_PS4); // initialize SPI

    // hardware reset: RST pin LOW then HIGH
    PIN_write(g_ili9341.rst, PIN_LOW);
    delay(100);
    PIN_write(g_ili9341.rst, PIN_HIGH);
    delay(150);

    ILI9341_reset(); // software reset

    ILI9341_setup_rgb();

    // turn on
    ILI9341_sleep_out();
    ILI9341_enable_display();
}

//------------------------------------------------------------------------------
// ILI9341_reset
//------------------------------------------------------------------------------

void ILI9341_reset(void)
{
    ILI9341_set_command(ILI9341_SPI_SWRESET);
    delay(120); // wait before sending new command
}

//------------------------------------------------------------------------------
// ILI9341_draw_pixel
//------------------------------------------------------------------------------

void ILI9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    ILI9341_define_area(x, y, 1, 1);
    ILI9341_set_data16(color);
}

//------------------------------------------------------------------------------
// ILI9341_fill_screen
//------------------------------------------------------------------------------

void ILI9341_fill_screen(color16_t color)
{
    ILI9341_fill_area(0, 0, g_ili9341.w, g_ili9341.h, color);
}

//------------------------------------------------------------------------------
// ILI9341_fill_area
//------------------------------------------------------------------------------

void ILI9341_fill_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color16_t color)
{
    ILI9341_define_area(x, y, w, h);
    for (uint32_t i = 0; i < ILI9341_UTIL_SIZE(w, h); ++i)
    {
        ILI9341_set_data16(color);
    }
}

//------------------------------------------------------------------------------
// ILI9341_set_orientation
//------------------------------------------------------------------------------

void ILI9341_set_orientation(ili9341_orientation_t orientation)
{
    BIT_write(g_ili9341.madctl, orientation, ILI9341_MASK_ORIENTATION);
    if (BIT_is_clear(orientation, ILI9341_MADCTL_MV))
    {
        g_ili9341.w = ILI9341_WIDTH;
        g_ili9341.h = ILI9341_HEIGHT;
    } // if portrait
    else
    {
        g_ili9341.w = ILI9341_HEIGHT;
        g_ili9341.h = ILI9341_WIDTH;
    } // if landscape
    ILI9341_set_command(ILI9341_SPI_MADCTL);
    ILI9341_set_data(g_ili9341.madctl);
}

//------------------------------------------------------------------------------
// ILI9341_set_text_size
//------------------------------------------------------------------------------

void ILI9341_set_text_size(length_t size)
{
    g_ili9341.text.size = size;
}

//------------------------------------------------------------------------------
// ILI9341_set_text_spacing
//------------------------------------------------------------------------------

void ILI9341_set_text_spacing(length_t spacing)
{
    g_ili9341.text.spacing = spacing;
}

//------------------------------------------------------------------------------
// ILI9341_set_text_color
//------------------------------------------------------------------------------

void ILI9341_set_text_color(color16_t color)
{
    g_ili9341.text.color = color;
}

//------------------------------------------------------------------------------
// ILI9341_set_text_background
//------------------------------------------------------------------------------

void ILI9341_set_text_background(color16_t background)
{
    g_ili9341.text.background = background;
}

static inline byte_t ILI9341_font(uint16_t ch, uint16_t idx)
{
    if (FONT_WIDTH > idx)
    {
        return (pgm_read_byte(&font[ch + idx]));
    }
    return (0);
}

//------------------------------------------------------------------------------
// ILI9341_draw_char
//------------------------------------------------------------------------------

void ILI9341_draw_char(uint16_t x, uint16_t y, char ch)
{
    ILI9341_define_area(x, y,
                        FONT_WIDTH * g_ili9341.text.size,
                        FONT_HEIGHT * g_ili9341.text.size);
    uint16_t font_idx = (ch - 32) * FONT_WIDTH;

    for (uint8_t row = 0; row < FONT_HEIGHT; ++row)
    {
        for (uint8_t i = 0; i < g_ili9341.text.size; ++i)
        {
            for (uint8_t col = 0; col < FONT_WIDTH; ++col)
            {
                byte_t line = (col < 5)
                                   ? pgm_read_byte(&font[font_idx + col])
                                   : 0x00;
                color16_t color = (BIT_read(line, BIT(row)))
                                      ? g_ili9341.text.color
                                      : g_ili9341.text.background;

                for (uint8_t j = 0; j < g_ili9341.text.size; ++j)
                {
                    ILI9341_set_data16(color);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// ILI9341_draw_string
//------------------------------------------------------------------------------

void ILI9341_draw_string(uint16_t x, uint16_t y, const char *str)
{
    while ('\0' != *str)
    {
        ILI9341_draw_char(x, y, *str);
        ++str;
        x += (g_ili9341.text.size * FONT_WIDTH) + g_ili9341.text.spacing;
    }
}

// void ILI9341_draw_char(uint16_t x, uint16_t y, char c, color16_t color, color16_t bg, uint8_t size)
// {
//     if (size < 1)
//     {
//         size = 1;
//     }

//     ILI9341_define_area(x, y, (6 * size), (8 * size));

//     uint16_t font_idx = (c - 32) * 5;

//     for (uint8_t col = 0; col < 6; col++)
//     {
//         uint8_t line = (col < 5) ? pgm_read_byte(&font[font_idx + col]) : 0x00;

//         // Horizontal scaling: Repeat the column 'size' times
//         for (uint8_t s_col = 0; s_col < size; s_col++)
//         {

//             for (uint8_t row = 0; row < 8; row++)
//             {
//                 uint16_t active_color = (line & (1 << row)) ? color : bg;

//                 // Vertical scaling: Repeat the pixel 'size' times
//                 for (uint8_t s_row = 0; s_row < size; s_row++)
//                 {
//                     ILI9341_set_data(active_color);
//                 }
//             }
//         }
//     }
// }
