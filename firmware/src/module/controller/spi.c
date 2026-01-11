#include "spi.h"

#define SPI_SS 0x04 ///< SPI Slave Select
#define SPI_MOSI 0x08 ///< SPI Master Output Slave Input
#define SPI_MISO 0x10 ///< SPI Master Input Slave Output
#define SPI_SCK 0x20 ///< SPI Clock Input

void SPI_master_init(byte_t clock)
{
	// Set MOSI, SCK output, all others input
	BIT_set(DDRB, SPI_MOSI);
	BIT_set(DDRB, SPI_SCK);
	BIT_set(DDRB, SPI_SS);
	BIT_clear(DDRB, SPI_MISO);
	SPI_enable();
	SPI_enable_master();
	SPI_set_clock(clock);
}

void SPI_slave_init(void)
{
	// Set MISO output, all others input
	BIT_set(DDRB, SPI_MISO);
	BIT_clear(DDRB, SPI_MOSI);
	BIT_clear(DDRB, SPI_SCK);
	BIT_clear(DDRB, SPI_SS);
	SPI_enable();
	SPI_enable_slave();
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

byte_t SPI_slave_receive(void)
{
	WAIT_UNTIL(BIT_read(SPSR, BIT(SPIF)));
	return SPDR;
}
