#ifndef VEMAR_SPI_H
#define VEMAR_SPI_H

#include "common.h"

#define SPI_CLOCK_4 0x00 ///< F_osc / 4
#define SPI_CLOCK_16 0x01 ///< F_osc / 16
#define SPI_CLOCK_64 0x02 ///< F_osc / 64
#define SPI_CLOCK_128 0x03 ///< F_osc / 128

#define SPI_MASK_CLOCK 0x03

/**
 * @brief Enable SPI
 *
 * When the `SPE` bit written to `1`, the SPI is enabled.
 * This bit must be set to enable any SPI operations.
 */
inline void SPI_enable(void) {
	BIT_set(SPCR, BIT(SPE));
}

/**
 * @brief Disable SPI
 */
inline void SPI_disable(void) {
	BIT_clear(SPCR, BIT(SPE));
}

/**
 * @brief Enable SPI interrupt
 * @details
 * This bit causes the SPI interrupt to be executed if `SPIF` bit in the `SPSR`
 * register is set and if the global interrupt enable bit in `SREG` is set.
 */
inline void SPI_enable_interrupt(void) {
	BIT_set(SPCR, BIT(SPIE));
}

/**
 * @brief Disable SPI interrupt
 */
inline void SPI_disable_interrupt(void) {
	BIT_clear(SPCR, BIT(SPIE));
}

/**
 * @brief Enable SPI master mode
 *
 * This bit selects master SPI mode when written to `1`, and slave SPI mode
 * when written logic `0`. If `SS` is configured as an input and is driven low
 * while `MSTR` is set, `MSTR` will be cleared, and `SPIF` in `SPSR` will become
 * set. The user will then have to set `MSTR` to re-enable SPI master mode.
 */
inline void SPI_enable_master(void) {
	BIT_set(SPCR, BIT(MSTR));
}

/**
 * @brief Enable SPI slave mode
 */
inline void SPI_enable_slave(void) {
	BIT_clear(SPCR, BIT(MSTR));
}

/**
 * @brief Set SPI clock rate
 * @param clock Clock rate
 * - SPI_CLOCK_4
 * - SPI_CLOCK_16
 * - SPI_CLOCK_64
 * - SPI_CLOCK_128
 *
 * These two bits control the `SCK` rate of the device configured as a master.
 * `SPR1` and `SPR0` have no effect on the slave.
 */
inline void SPI_set_clock(byte_t clock) {
	BIT_write(SPCR, clock, SPI_MASK_CLOCK);
}

void SPI_master_init(byte_t clock);

void SPI_master_transmit(byte_t data);

void SPI_slave_init(void);

byte_t SPI_slave_receive(void);

#endif // VEMAR_SPI_H
