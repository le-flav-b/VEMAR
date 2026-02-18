#include "spi.h"

#define SPI_SS 0x04   ///< SPI Slave Select
#define SPI_MOSI 0x08 ///< SPI Master Output Slave Input
#define SPI_MISO 0x10 ///< SPI Master Input Slave Output
#define SPI_SCK 0x20  ///< SPI Clock Input

// void SPI_master_init(byte_t order, byte_t mode, byte_t clock)
// {
//     // MOSI, SCK as output
//     BIT_set(DDRB, SPI_MOSI);
//     BIT_set(DDRB, SPI_SCK);
//     BIT_set(DDRB, SPI_SS);
//     BIT_clear(DDRB, SPI_MISO);

//     // Enable SPI, set as Master
//     SPCR = (BIT(SPE) | BIT(MSTR) | (order) | (mode) | (clock));
// }

void SPI_init(void)
{
    static bool_t init = 0;

    if (0 == init)
    {
        // MOSI, SCK as output
        BIT_set(DDRB, SPI_MOSI);
        BIT_set(DDRB, SPI_SCK);
        BIT_set(DDRB, SPI_SS);
        BIT_clear(DDRB, SPI_MISO);

        // Enable SPI, set as Master
        SPCR = (BIT(SPE) | BIT(MSTR) | (SPI_CLOCK_64));
        init = 1;
    }
}

void SPI_transmit(byte_t data)
{
    SPDR = data;
    WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
}

byte_t SPI_receive(void)
{
    SPDR = 0xFF; // Dummy data
    WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
    return (SPDR);
}

void SPI_write(const byte_t *data, byte_t len)
{
    for (byte_t pos = 0; pos < len; ++pos)
    {
        SPDR = data[pos]; // Load data
        WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
    }
}

void SPI_read(byte_t *data, byte_t len)
{
    for (byte_t pos = 0; pos < len; ++pos)
    {
        SPDR = 0xFF; // Dummy data
        WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
        data[pos] = SPDR;
    }
}

// void SPI_master_transmit(byte_t data)
// {
//     SPDR = data; // Load data
//     WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
// }

// byte_t SPI_master_receive(void)
// {
//     SPDR = 0xFF; // Dummy data
//     WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
//     return SPDR;
// }
