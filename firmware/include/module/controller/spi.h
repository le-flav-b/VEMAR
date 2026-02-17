#ifndef VEMAR_SPI_H
#define VEMAR_SPI_H

#include "common.h"

/**
 * @defgroup spi_clock SPI Clock Divider
 * @brief Available clock divider selections for SPI
 * @{
 */
#define SPI_CLOCK_4 0x00   ///< F_osc / 4
#define SPI_CLOCK_16 0x01  ///< F_osc / 16
#define SPI_CLOCK_64 0x02  ///< F_osc / 64
#define SPI_CLOCK_128 0x03 ///< F_osc / 128
/**
 * @}
 */

/**
 * @defgroup spi_order SPI Data Order
 * @brief LSB first or MSB first for SPI transmission
 * @{
 */
#define SPI_ORDER_MSB 0x00 ///< SPI Data Order MSB
#define SPI_ORDER_LSB 0x20 ///< SPI Data Order LSB
/**
 * @}
 */

/**
 * @defgroup spi_polarity SPI Polarity
 * @brief Available polarity selections for SPI
 * @{
 */
#define SPI_POLARITY_LEADING 0x00  ///< Rising on leading edge
#define SPI_POLARITY_TRAILING 0x08 ///< Rising on trailing edge
/**
 * @}
 */

/**
 * @defgroup spi_phase SPI Phase
 * @brief Available phase selections for SPI
 * @{
 */
#define SPI_PHASE_LEADING 0x00	///< Sample on leading edge
#define SPI_PHASE_TRAILING 0x04 ///< Sample on trailing edge
/**
 * @}
 */

/**
 * @defgroup spi_mode SPI Mode
 * @brief Available mode selections for SPI
 * @details
 * | Mode | CPOL | CPHA |   Leading Edge   |   Trailing Edge  |
 * | ---- | ---- | ---- | ---------------- | ---------------- |
 * |   0  |   0  |   0  | Sample (Rising)  |  Setup (Falling) |
 * |   1  |   0  |   1  |  Setup (Rising)  | Sample (Falling) |
 * |   2  |   1  |   0  | Sample (Falling) |  Setup (Rising)  |
 * |   3  |   1  |   1  |  Setup (Falling) | Sample (Rising)  |
 * @{
 */
#define SPI_MODE_0 (SPI_POLARITY_LEADING | SPI_PHASE_LEADING)	///< SPI Mode 0
#define SPI_MODE_1 (SPI_POLARITY_LEADING | SPI_PHASE_TRAILING)	///< SPI Mode 1
#define SPI_MODE_2 (SPI_POLARITY_TRAILING | SPI_PHASE_LEADING)	///< SPI Mode 2
#define SPI_MODE_3 (SPI_POLARITY_TRAILING | SPI_PHASE_TRAILING) ///< SPI Mode 3
/**
 * @}
 */

/**
 * @{
 * @brief SPI Masks for internal use
 */
#define SPI_MASK_MODE 0x0C
#define SPI_MASK_CLOCK 0x03
/**
 * @}
 */

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
 * @brief Set SPI mode
 * @param mode
 */
inline void SPI_set_mode(byte_t mode)
{
	BIT_write(SPCR, mode, BIT(MSTR));
}

/**
 * @brief Set SPI Polarity
 * @param polarity SPI polarity
 * @details
 * When `CPOL` is written to `1`, SCK is high when idle.
 * When `CPOL` is written to `0`, SCK is low when idle.
 * @see spi_polarity
 */
inline void SPI_set_polarity(byte_t polarity)
{
	BIT_write(SPCR, polarity, BIT(CPOL));
}

/**
 * @brief Set SPI Phase
 * @param phase SPI phase
 * @details
 * The settings of the Clock Phase bit (`CPHA`) determine if data is sampled
 * on the leading (first) or trailing (last) edge of SCK.
 * @see spi_phase
 */
inline void SPI_set_phase(byte_t phase)
{
	BIT_write(SPCR, phase, BIT(CPHA));
}

/**
 * @brief Set SPI clock divider
 * @param clock Clock divider
 * @details
 * `SPR[1:0]` bits control the `SCK` rate of the device configured as a master.
 * `SPR1` and `SPR0` have no effect on the slave.
 * @see spi_clock
 */
inline void SPI_set_clock(byte_t clock)
{
	BIT_write(SPCR, clock, SPI_MASK_CLOCK);
}

//------------------------------------------------------------------------------
// SPSR --- SPI Status Register
// | SPIF | WCOL | --- | --- | --- | --- | --- | SPI2X |
//------------------------------------------------------------------------------

/**
 * @brief Check whether a serial transfer is complete
 * @return Non-zero value if the transfer is complete, otherwise `0`
 * @details
 * When a serial transfer is complete, the `SPIF` flag is set.
 * An interrupt is generated if `SPIE` in `SPCR` is set and Global Interrupts
 * are enabled.
 * If `~SS` is an input and is driven low when the `SPI` is in Master mode,
 * this will also set the `SPIF` flag.
 * `SPIF` is cleared by hardware when executing the corresponding interrupt
 * handling vector.
 * Alternatively, the `SPIF` bit is cleared by first reading the SPI Status
 * Register with `SPIF` set, then accessing the SPI Data Register (SPDR).
 */
inline bool_t SPI_is_transfert_complete(void)
{
	return (0 != BIT_read(SPSR, BIT(SPIF)));
}

/**
 * @brief Enable SPI double speed
 * @details
 * When `SPI2X` is written logic `1` the SPI speed (SCK Frequency) will be
 * doubled when the SPI is in Master mode.
 * This means that the minimum SCK period will be two CPU clock periods.
 * When the SPI is configured as Slave, the SPI is only ensured to work at
 * `f_osc / 4` or lower.
 */
inline void SPI_enable_double_speed(void)
{
	BIT_set(SPSR, BIT(SPI2X));
}

inline void SPI_disable_double_speed(void)
{
	BIT_clear(SPSR, BIT(SPI2X));
}

//------------------------------------------------------------------------------
// Quick configuration
//------------------------------------------------------------------------------

/**
 * @brief Initialize SPI as master (MSB, Mode 0, clock 64)
 */
void SPI_init(void);

void SPI_transmit(byte_t data);

/**
 * @brief Transmit data
 * @param data Data to transmit
 * @param len Size of the data to transmit
 */
void SPI_write(const byte_t *data, byte_t len);

byte_t SPI_receive(void);

/**
 * @brief Receive data
 * @param data Buffer to store received data
 * @param len Size of the data to receive
 */
void SPI_read(byte_t *data, byte_t len);

#endif // VEMAR_SPI_H

/**
 * @file spi.h
 * @brief SPI header file
 * @author Christian Hugon
 * @version 0.0.1
 */
