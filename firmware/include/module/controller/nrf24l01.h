#ifndef VEMAR_NRF24L01_H
#define VEMAR_NRF24L01_H

#include "gpio.h"

#define NRF24L01

/**
 *
 * Register Map table
 * @see datasheet section 9.1
 */
#define NRF24L01_CONFIG 0x00      ///< Configuration Register
#define NRF24L01_EN_AA 0x01       ///< Enable 'Auto-Acknowledgment'
#define NRF24L01_EN_RXADDR 0x02   ///< Enabled RX Addresses
#define NRF24L01_SETUP_AW 0x03    ///< Setup of Address Widths
#define NRF24L01_SETUP_RETR 0x04  ///< Setup of Automatic Retransmission
#define NRF24L01_RF_CH 0x05       ///< RF Channel
#define NRF24L01_RF_SETUP 0x06    ///< RF Setup Register
#define NRF24L01_STATUS 0x07      ///< Status Register
#define NRF24L01_OBSERVE_TX 0x08  ///< Transmit Observe register
#define NRF24L01_CD 0x09          ///< Carrier Detect
#define NRF24L01_RX_ADDR_P0 0x0A  ///< Receive address data pipe 0
#define NRF24L01_RX_ADDR_P1 0x0B  ///< Receive address data pipe 1
#define NRF24L01_RX_ADDR_P2 0x0C  ///< Receive address data pipe 2
#define NRF24L01_RX_ADDR_P3 0x0D  ///< Receive address data pipe 3
#define NRF24L01_RX_ADDR_P4 0x0E  ///< Receive address data pipe 4
#define NRF24L01_RX_ADDR_P5 0x0F  ///< Receive address data pipe 5
#define NRF24L01_TX_ADDR 0x10     ///< Transmit address
#define NRF24L01_RX_PW_P0 0x11    ///< Number of bytes in RX payload in data pipe 0
#define NRF24L01_RX_PW_P1 0x12    ///< Number of bytes in RX payload in data pipe 1
#define NRF24L01_RX_PW_P2 0x13    ///< Number of bytes in RX payload in data pipe 2
#define NRF24L01_RX_PW_P3 0x14    ///< Number of bytes in RX payload in data pipe 3
#define NRF24L01_RX_PW_P4 0x15    ///< Number of bytes in RX payload in data pipe 4
#define NRF24L01_RX_PW_P5 0x16    ///< Number of bytes in RX payload in data pipe 5
#define NRF24L01_FIFO_STATUS 0x17 ///< FIFO Status Register
#define NRF24L01_DYNPD 0x1C       ///< Enable dynamic payload length
#define NRF24L01_FEATURE 0x1D     ///< Feature Register

#define NRF24L01_SPI_R_REGISTER 0x00         ///< Read command and `STATUS` register
#define NRF24L01_SPI_W_REGISTER 0x20         ///< Write command and `STATUS` register
#define NRF24L01_SPI_R_RX_PAYLOAD 0x61       ///< Read RX payload
#define NRF24L01_SPI_W_TX_PAYLOAD 0xA0       ///< Write TX payload
#define NRF24L01_SPI_FLUSH_TX 0xE1           ///< Flush TX FIFO, used in TX mode
#define NRF24L01_SPI_FLUSH_RX 0xE2           ///< Flush RX FIFO, used in RX mode
#define NRF24L01_SPI_REUSE_TX_PL 0xE3        ///< Reuse last transmitted payload
#define NRF24L01_SPI_ACTIVATE 0x50           ///< Activate `R_RX_PL_WID`, `W_ACK_PAYLOAD`, `W_TX_PAYLOAD_NOACK`
#define NRF24L01_SPI_R_RX_PL_WID 0x60        ///< Read RX payload width
#define NRF24L01_SPI_W_ACK_PAYLOAD 0xA8      ///< Write Payload to be transmitted
#define NRF24L01_SPI_W_TX_PAYLOAD_NOACK 0xB0 ///< Disable `AUTOACK` on this specific packet
#define NRF24L01_SPI_NOP 0xFF                ///< No Operation

typedef enum nrf24l01_pipe
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


void NRF24L01_debug_config(void);

/**
 * @brief Initialize NRF24L01 chip
 * @param ce Chip Enable pin
 * @param csn Chip Select pin
 */
void NRF24L01_init(pin_t ce, pin_t csn);

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

void NRF24L01_print(void);

/**
 * @brief Return the value of the STATUS register
 */
byte_t NRF24L01_status(void);

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

// void NRF24L01_set_data_rate(data_rate_t rate);

void NRF24L01_power_up(void);

void NRF24L01_power_down(void);

void NRF24L01_mode_rx(void);
void NRF24L01_mode_tx(void);
void NRF24L01_standby(void);

void NRF24L01_flush_tx(void);
void NRF24L01_flush_rx(void);
// void NRF24L01_activate(void);

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
 * @brief Set NRF24L01 to Primary TX mode
 */
void NRF24L01_primary_tx(void);

/**
 * @brief Set NRF24L01 to Primary RX mode
 */
void NRF24L01_primary_rx(void);

/**
 * @brief Set the payload size
 * @param pipe Data pipe
 * @param size Number of bytes in RX payload (1 to 32 bytes)
 *
 * @note The parameter `size` @b must be in range of [1:32]
 */
void NRF24L01_set_payload_size(pipe_t pipe, length_t size);

/**
 * @brief Disable the specified data pipe
 * @param pipe Data pipe
 */
void NRF24L01_disable_pipe(pipe_t pipe);

/**
 * @brief Set the number of attempts of retransmission
 * @param count Auto-retransmit count (1 to 15)
 * @param delay Delay between 2 retransmissions (0 to 15),
 * the delay is calculated as following: `(delay + 1) * 250` in microseconds
 */
void NRF24L01_set_retransmit(length_t count, length_t delay);

/**
 * @brief Disable automatic retransmission
 */
void NRF24L01_disable_retransmit(void);

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
 * @brief Set the Power Amplifier (PA) settings
 * @param level PA level
 */
void NRF24L01_set_pa(pa_t level);

/**
 * @brief Enable Low Noise Amplifier
 */
void NRF24L01_enable_lna(void);

/**
 * @brief Disable Low Noise Amplifier
 */
void NRF24L01_disable_lna(void);

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

#endif // VEMAR_NRF24L01_H

/**
 * @file nrf24l01.h
 * @brief NRF24L01 module
 * @author Christian Hugon
 *
 * Not implemented:
 * - Dynamic payload
 * - Interrupt
 */
