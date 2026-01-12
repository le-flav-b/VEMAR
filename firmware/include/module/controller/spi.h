#ifndef VEMAR_SPI_H
#define VEMAR_SPI_H

#include "common.h"

#define SPI_CLOCK_4 0x00   ///< F_osc / 4
#define SPI_CLOCK_16 0x01  ///< F_osc / 16
#define SPI_CLOCK_64 0x02  ///< F_osc / 64
#define SPI_CLOCK_128 0x03 ///< F_osc / 128

#define SPI_MASK_CLOCK 0x03

#define SPI_ORDER_LSB 0x20 ///< SPI Data Order LSB
#define SPI_ORDER_MSB 0x00 ///< SPI Data Order MSB

#define SPI_MODE_MASTER 0x10 ///< SPI Master Mode
#define SPI_MODE_SLAVE 0x00  ///< SPI Slave Mode

#define SPI_POLARITY_LEADING
#define SPI_POLARITY_TRAILING

#define SPI_PHASE_LEADING
#define SPI_PHASE_TRAILING

//------------------------------------------------------------------------------
// SPCR --- SPI Control Register
// | SPIE | SPE | DORD | MSTR | CPOL | CPHA | SPR1 | SPR0 |
//------------------------------------------------------------------------------

/**
 * @brief Enable SPI interrupt
 * @details
 * This bit causes the SPI interrupt to be executed if `SPIF` bit in the `SPSR`
 * register is set and if the Global Interrupt Enable bit in `SREG` is set.
 */
inline void SPI_enable_interrupt(void)
{
    BIT_set(SPCR, BIT(SPIE));
}

/**
 * @brief Disable SPI interrupt
 */
inline void SPI_disable_interrupt(void)
{
    BIT_clear(SPCR, BIT(SPIE));
}

/**
 * @brief Enable SPI
 *
 * When the `SPE` bit written to `1`, the SPI is enabled.
 * This bit must be set to enable any SPI operations.
 */
inline void SPI_enable(void)
{
    BIT_set(SPCR, BIT(SPE));
}

/**
 * @brief Disable SPI
 */
inline void SPI_disable(void)
{
    BIT_clear(SPCR, BIT(SPE));
}

/**
 * @brief Set SPI Data order
 * @param order
 * - SPI_ORDER_LSB
 * - SPI_ORDER_MSB
 * @details
 * When the `DORD` bit is written to `1`, the LSB of the data word is
 * transmitted first.
 * When the `DORD` bit is written to `0`, the MSB of the data word is
 * transmitted first.
 */
inline void SPI_set_order(byte_t order)
{
    BIT_write(SPCR, order, BIT(DORD));
}

/**
 * @brief Enable SPI master mode
 * @param mode
 * - SPI_MODE_MASTER
 * - SPI_MODE_SLAVE
 * @details
 * This bit selects master SPI mode when written to `1`, and slave SPI mode
 * when written logic `0`.
 * If `~SS` is configured as an input and is driven low while `MSTR` is set,
 * `MSTR` will be cleared, and `SPIF` in `SPSR` will become set.
 * The user will then have to set `MSTR` to re-enable SPI master mode.
 */
inline void SPI_set_mode(byte_t mode)
{
    BIT_write(SPCR, mode, BIT(MSTR));
}

// inline void SPI_enable_master(void)
// {
//     BIT_set(SPCR, BIT(MSTR));
// }

// /**
//  * @brief Enable SPI slave mode
//  */
// inline void SPI_enable_slave(void)
// {
//     BIT_clear(SPCR, BIT(MSTR));
// }

/**
 * @param polarity
 * @details
 * When `CPOL` is written to `1`, SCK is high when idle.
 * When `CPOL` is written to `0`, SCK is low when idle.
 */
inline void SPI_set_polarity(byte_t polarity)
{
    BIT_write(SPCR, polarity, BIT(CPOL));
}

/**
 * @param phase
 * @details
 * The settings of the Clock Phase bit (`CPHA`) determine if data is sampled
 * on the leading (first) or trailing (last) edge of SCK.
 */
inline void SPI_set_phase(byte_t phase)
{
    BIT_write(SPCR, phase, BIT(CPHA));
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
inline void SPI_set_clock(byte_t clock)
{
    BIT_write(SPCR, clock, SPI_MASK_CLOCK);
}

//------------------------------------------------------------------------------
// SPSR --- SPI Status Register
// | SPIF | WCOL | --- | --- | --- | --- | --- | SPI2X |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void SPI_master_init(byte_t clock);

void SPI_master_transmit(byte_t data);

void SPI_slave_init(void);

byte_t SPI_slave_receive(void);

#endif // VEMAR_SPI_H

/**
 * @file spi.h
 * @brief SPI header file
 * @author Christian Hugon
 * @version 0.0.1
 */
