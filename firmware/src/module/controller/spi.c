#include "spi.h"

#define SPI_SS 0x04   ///< SPI Slave Select
#define SPI_MOSI 0x08 ///< SPI Master Output Slave Input
#define SPI_MISO 0x10 ///< SPI Master Input Slave Output
#define SPI_SCK 0x20  ///< SPI Clock Input

void SPI_master_init(byte_t order, byte_t mode, byte_t clock)
{
    // MOSI, SCK as output
    BIT_set(DDRB, SPI_MOSI);
    BIT_set(DDRB, SPI_SCK);
    BIT_set(DDRB, SPI_SS);
    BIT_clear(DDRB, SPI_MISO);

    // Enable SPI, set as Master
    SPCR = (BIT(SPE) | BIT(MSTR) | (order) | (mode) | (clock));
}

void SPI_master_transmit(byte_t data)
{
    SPDR = data; // Load data
    WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
}

byte_t SPI_master_receive(void)
{
    SPDR = 0xFF; // Dummy data
    WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
    return SPDR;
}
