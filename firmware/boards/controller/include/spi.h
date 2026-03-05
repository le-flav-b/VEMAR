#ifndef VEMAR_SPI_H
#define VEMAR_SPI_H

#include "common.h"

//------------------------------------------------------------------------------
// Enumerations
//------------------------------------------------------------------------------

/**
 * @brief Define SPI data order
 */
typedef enum
{
    SPI_MSB = 0x00, /**< MSB first */
    SPI_LSB = 0x20  /**< LSB first */
} spi_order_t;

/**
 * @brief Define SPI mode (Polarity & Phase)
 * @details
 * | Mode | CPOL | CPHA |   Leading Edge   |   Trailing Edge  |
 * | ---- | ---- | ---- | ---------------- | ---------------- |
 * |   0  |   0  |   0  | Sample (Rising)  |  Setup (Falling) |
 * |   1  |   0  |   1  |  Setup (Rising)  | Sample (Falling) |
 * |   2  |   1  |   0  | Sample (Falling) |  Setup (Rising)  |
 * |   3  |   1  |   1  |  Setup (Falling) | Sample (Rising)  |
 */
typedef enum
{
    SPI_MODE0 = 0x00, /**< Sample on rising, Setup on falling */
    SPI_MODE1 = 0x04, /**< Setup on rising, Sample on falling */
    SPI_MODE2 = 0x08, /**< Sample on falling, Setup on rising */
    SPI_MODE3 = 0x0C  /**< Setup on falling, Sample on rising */
} spi_mode_t;

/**
 * @brief Define SPI prescaler
 */
typedef enum
{
    SPI_PS4 = 0x00,   /**< F_osc / 4 */
    SPI_PS16 = 0x01,  /**< F_osc / 16 */
    SPI_PS64 = 0x02,  /**< F_osc / 64 */
    SPI_PS128 = 0x03, /**< F_osc / 128 */
    SPI_PS2 = 0x10,   /**< F_osc / 2 */
    SPI_PS8 = 0x11,   /**< F_osc / 8 */
    SPI_PS32 = 0x12,  /**< F_osc / 32 */
} spi_ps_t;

/**
 * @brief Initialize SPI as master
 * @param order Data order
 * @param mode SPI mode
 * @param prescaler Clock division
 * @see spi_order_t
 * @see spi_mode_t
 * @see spi_ps_t
 */
void SPI_init(spi_order_t order, spi_mode_t mode, spi_ps_t prescaler);

/**
 * @brief Receive one byte of data
 * @return Received byte
 */
byte_t SPI_receive(void);

/**
 * @brief Transmit one byte of data
 * @param data Byte to tramsmit
 */
void SPI_transmit(byte_t data);

/**
 * @brief Transmit a serie of data
 * @param src Data buffer to transmit
 * @param len Size of the data buffer to transmit
 */
void SPI_write(const byte_t *src, length_t len);

/**
 * @brief Receive a serie of data
 * @param data Data buffer to store received data
 * @param len Size of the data to receive
 */
void SPI_read(byte_t *dst, length_t len);

//------------------------------------------------------------------------------
// Advanced configuration
//------------------------------------------------------------------------------

/**
 * @brief Reset SPI registers to their defalut values
 */
void SPI_reset(void);

/**
 * @brief Check whether the SPI is enabled
 * @return `TRUE` if SPI is enabled, otherwise `FALSE`
 */
inline bool_t SPI_is_enabled(void)
{
    return (BIT_is_set(SPCR, BIT(SPE)));
}

/**
 * @brief Enable SPI
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
 * @brief Enable SPI interrupt
 * @warning SPI interruption not available
 */
inline void SPI_enable_interrupt(void)
{
    BIT_set(SPCR, BIT(SPIE));
}

/**
 * @brief Disable SPI interrupt
 * @warning SPI interruption not available
 */
inline void SPI_disable_interrupt(void)
{
    BIT_clear(SPCR, BIT(SPIE));
}

/**
 * @brief Return SPI data order
 * @return SPI data order
 * @see spi_order_t
 */
inline spi_order_t SPI_get_order(void)
{
    return (BIT_read(SPCR, BIT(DORD)));
}

/**
 * @brief Set SPI Data order
 * @param order Data order
 */
inline void SPI_set_order(spi_order_t order)
{
    BIT_write(SPCR, order, BIT(DORD));
}

/**
 * @brief Return SPI mode
 * @return SPI mode
 * @see spi_mode_t
 */
inline spi_mode_t SPI_get_mode(void)
{
    return (BIT_read(SPCR, (BIT(CPOL) | BIT(CPHA))));
}

/**
 * @brief Set SPI mode
 * @param mode SPI mode to set
 * @see spi_mode_t
 */
inline void SPI_set_mode(spi_mode_t mode)
{
    BIT_write(SPCR, mode, (BIT(CPOL) | BIT(CPHA)));
}

/**
 * @brief Return the SPI prescaler
 * @return SPI prescaler
 */
spi_ps_t SPI_get_prescaler(void);

/**
 * @brief Set SPI clock divider
 * @param prescaler Clock divider
 * @see spi_ps_t
 */
void SPI_set_prescaler(spi_ps_t prescaler);

/**
 * @brief Check whether a serial transfer is complete
 * @return `TRUE` if the transfer is complete, otherwise `FALSE`
 */
inline bool_t SPI_is_complete(void)
{
    return (BIT_is_set(SPSR, BIT(SPIF)));
}

#endif // VEMAR_SPI_H

/**
 * @file spi.h
 * @brief SPI utility functions
 * @author Christian Hugon <chriss.hugon@gmail.com>
 * @version 1.0.0
 * @details
 * For quick configuration
 * ```
 * void setup(void)
 * {
 *      SPI_init(SPI_MSB, SPI_MODE0, SPI_SP16);
 * }
 *
 * void loop(void)
 * {
 *      SPI_transmit(0x00);
 *      byte_t res = SPI_receive();
 *      // handle res
 *      delay(1000);
 * }
 * ```
 */
