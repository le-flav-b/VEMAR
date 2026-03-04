#include "spi.h"

#define SPI_SS 0x04   ///< SPI Slave Select
#define SPI_MOSI 0x08 ///< SPI Master Output Slave Input
#define SPI_MISO 0x10 ///< SPI Master Input Slave Output
#define SPI_SCK 0x20  ///< SPI Clock Input

#define _SPI_MASK_PRESCALER 0x03

//------------------------------------------------------------------------------
// SPI_init
//------------------------------------------------------------------------------
void SPI_init(spi_order_t order, spi_mode_t mode, spi_ps_t prescaler)
{
    if (!SPI_is_enabled())
    {
        // set MOSI, SCK as output, MISO as input
        BIT_set(DDRB, SPI_MOSI);
        BIT_set(DDRB, SPI_SCK);
        BIT_set(DDRB, SPI_SS);
        BIT_clear(DDRB, SPI_MISO);

        // enable SPI, set as Master
        SPCR = BIT(SPE) | (order) | BIT(MSTR) | (mode) |
               (prescaler & _SPI_MASK_PRESCALER);
        SPSR = (prescaler) >> 4; // write `SPI2X` bit if required
    } // if SPI is not initialized
}

//------------------------------------------------------------------------------
// SPI_transmit
//------------------------------------------------------------------------------
void SPI_transmit(byte_t data)
{
    SPDR = data;
    WAIT_UNTIL(SPI_is_complete());
}

//------------------------------------------------------------------------------
// SPI_receive
//------------------------------------------------------------------------------
byte_t SPI_receive(void)
{
    SPDR = 0xFF; // Dummy data
    WAIT_UNTIL(SPI_is_complete());
    return (SPDR);
}

//------------------------------------------------------------------------------
// SPI_write
//------------------------------------------------------------------------------
void SPI_write(const byte_t *data, length_t len)
{
    for (byte_t pos = 0; pos < len; ++pos)
    {
        SPDR = data[pos]; // Load data
        WAIT_UNTIL(SPI_is_complete());
    }
}

//------------------------------------------------------------------------------
// SPI_read
//------------------------------------------------------------------------------
void SPI_read(byte_t *data, length_t len)
{
    for (byte_t pos = 0; pos < len; ++pos)
    {
        SPDR = 0xFF; // Dummy data
        WAIT_UNTIL(SPI_is_complete());
        data[pos] = SPDR;
    }
}

//------------------------------------------------------------------------------
// SPI_get_prescaler
//------------------------------------------------------------------------------
spi_ps_t SPI_get_prescaler(void)
{
    return ((spi_ps_t)((BIT_read(SPSR, BIT(SPI2X)) << 4) |
                       (BIT_read(SPCR, _SPI_MASK_PRESCALER))));
}

//------------------------------------------------------------------------------
// SPI_set_prescaler
//------------------------------------------------------------------------------
void SPI_set_prescaler(spi_ps_t prescaler)
{
    BIT_write(SPCR, prescaler, _SPI_MASK_PRESCALER);
    BIT_write(SPSR, (prescaler >> 4), BIT(SPI2X));
}

//------------------------------------------------------------------------------
// SPI_reset
//------------------------------------------------------------------------------
void SPI_reset(void)
{
    SPCR = 0x00;
    SPSR = 0x00;
}

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
extern inline bool_t SPI_is_enabled(void);
extern inline bool_t SPI_is_complete(void);

extern inline void SPI_enable(void);
extern inline void SPI_enable_interrupt(void);

extern inline void SPI_disable(void);
extern inline void SPI_disable_interrupt(void);

extern inline spi_order_t SPI_get_order(void);
extern inline spi_mode_t SPI_get_mode(void);

extern inline void SPI_set_order(spi_order_t);
extern inline void SPI_set_mode(spi_mode_t);
