#ifndef VEMAR_NRF24L01_H
#define VEMAR_NRF24L01_H

#include "gpio.h"

#define DEFINE_NRF24L01

/**
 * @brief Define available data pipes
 */
typedef enum
{
    NRF24L01_PIPE_0 = 0,
    NRF24L01_PIPE_1 = 1,
    NRF24L01_PIPE_2 = 2,
    NRF24L01_PIPE_3 = 3,
    NRF24L01_PIPE_4 = 4,
    NRF24L01_PIPE_5 = 5
} pipe_t;

/**
 * @brief Power Amplifier Level
 *
 * |     Level     | RF Output Power | DC Current Consumption |
 * | ------------- | --------------- | ---------------------- |
 * | NRF24L01_PA_0 |      -18dBm     |          7.0mA         |
 * | NRF24L01_PA_1 |      -12dBm     |          7.5mA         |
 * | NRF24L01_PA_2 |       -6dBm     |          9.0mA         |
 * | NRF24L01_PA_3 |        0dBm     |         11.3mA         |
 */
typedef enum nrf24l01_enum_pa
{
    NRF24L01_PA_0 = 0,
    NRF24L01_PA_1 = 1,
    NRF24L01_PA_2 = 2,
    NRF24L01_PA_3 = 3,
} pa_t;

typedef enum nrf24l01_enum_status
{
    NRF24L01_MAX_RT = (1 << 4), ///< Maximum number of TX retransmits
    NRF24L01_TX_DS = (1 << 5),  ///< Packet transmitted on TX
    NRF24L01_RX_DR = (1 << 6),  ///< New data in RX FIFO
} status_t;

/**
 * @brief Air Data Rate
 */
typedef enum nrf24l01_enum_data_rate
{
    NRF24L01_1MBPS = 0x00, ///< 1Mbps
    NRF24L01_2MBPS = 0x08  ///< 2Mbps
} rf_rate_t;

/**
 * @brief RF output power in TX mode
 */
typedef enum nrf24l01_enum_power
{
    NRF24L01_N18DBM = 0x00, ///< -18dBm
    NRF24L01_N12DBM = 0x02, ///< -12dBm
    NRF24L01_N6DBM = 0x04,  ///< -6dBm
    NRF24L01_0DBM = 0x06    ///< 0dBm
} rf_power_t;

/**
 * @brief Initialize NRF24L01 chip
 * @param ce Chip Enable pin
 * @param csn Chip Select pin
 */
void NRF24L01_init(pin_t ce, pin_t csn);

/**
 * @brief Enable NRF24L01
 * - In RX mode, begin to listen
 * - In TX mode, begin to transmit
 */
void NRF24L01_enable(void);

/**
 * @brief Disable NRF24L01
 */
void NRF24L01_disable(void);

/**
 * @brief Set the data rate and the transmitter output power,
 * enable or disable the Low Noise Amplifier
 * @param rate Air data rate
 * @param power Output Power
 * @param lna `0` to disable LNA, otherwise enable it
 *
 * @see rf_rate_t
 * @see rf_power_t
 */
void NRF24L01_setup(rf_rate_t rate, rf_power_t power, bool_t lna);

/**
 * @brief Power up NRF24L01
 */
void NRF24L01_power_up(void);

/**
 * @brief Power down NRF24L01
 */
void NRF24L01_power_down(void);

/**
 * @brief Enter RX mode
 */
void NRF24L01_mode_rx(void);

/**
 * @brief Enter TX mode
 */
void NRF24L01_mode_tx(void);

/**
 * @brief Enter STANBY mode
 */
void NRF24L01_standby(void);

/**
 * @brief Check whether RX FIFO is empty
 */
bool_t NRF24L01_is_rx_empty(void);

/**
 * @brief Check whether RX FIFO is full
 */
bool_t NRF24L01_is_rx_full(void);

/**
 * @brief Check whether TX FIFO is empty
 */
bool_t NRF24L01_is_tx_empty(void);

/**
 * @brief Check whether TX FIFO is full
 */
bool_t NRF24L01_is_tx_full(void);

/**
 * @brief Check whether a payload has received
 * @return `TRUE` if a payload is available, otherwise `FALSE`
 */
bool_t NRF24L01_has_payload(void);

/**
 * @brief Return the value of the STATUS register
 */
byte_t NRF24L01_status(void);

/**
 * @brief Set the RF channel frequency
 * @param frequency The RF channel frequency (0 to 125),
 * the frequency is calculated as following: `2400 + frequency` in MHz
 *
 * @note The transmitter and the receiver must be programmed with the same
 * RF channel frequency to be able to communicate with each other
 */
void NRF24L01_set_frequency(length_t frequency);

/**
 * @brief Set address for transmission
 * @param addr Address for transmission
 */
void NRF24L01_set_address_tx(const byte_t *addr);

/**
 * @brief Set address for reception and enable the corresponding data pipe
 * @param pipe Data pipe for reception
 * @param addr Address for transmission
 */
void NRF24L01_set_address_rx(pipe_t pipe, const byte_t *addr);

/**
 * @brief Set the payload size
 * @param pipe Data pipe
 * @param size Number of bytes in RX payload (1 to 32 bytes)
 *
 * @note The parameter `size` @b must be in range of [1:32]
 */
void NRF24L01_set_payload_size(pipe_t pipe, length_t size);

/**
 * @brief Set the number of attempts of retransmission
 * @param count Auto-retransmit count (1 to 15)
 * @param delay Delay between 2 retransmissions (0 to 15),
 * the delay is calculated as following: `(delay + 1) * 250` in microseconds
 */
void NRF24L01_set_retransmit(length_t count, length_t delay);

/**
 * @brief Clear the STATUS register
 */
void NRF24L01_clear_status(void);

/**
 * @brief Read RX-payload.
 * @param buff Buffer to store payload
 * @param len Length of the payload (1 to 32 bytes)
 */
void NRF24L01_read_payload(byte_t *buff, length_t len);

/**
 * @brief Write TX-payload
 * @param buff Buffer containing payload to write
 * @param len Length of the payload (1 to 32 bytes)
 */
void NRF24L01_write_payload(const byte_t *buff, length_t len);

/**
 * @brief Flush RX buffer
 */
void NRF24L01_flush_rx(void);

/**
 * @brief Flush TX buffer
 */
void NRF24L01_flush_tx(void);

/**
 * @brief Display information about the NRF24L01
 */
void NRF24L01_print(void);

#endif // VEMAR_NRF24L01_H

/**
 * @file nrf24l01.h
 * @brief NRF24L01 module
 * @author Christian Hugon <chriss.hugon@gmail.com>
 */
