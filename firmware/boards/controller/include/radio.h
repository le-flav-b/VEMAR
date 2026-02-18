#ifndef VEMAR_RADIO_H
#define VEMAR_RADIO_H

#include "nrf24l01.h"
#include "gpio.h"

#if !defined(NRF24L01)
#error "Module 'NRF24L01' not defined"
#endif

// typedef struct vemar_struct_radio
// {
// 	byte_t address_rx[5]; ///< Address for reading
// 	byte_t address_tx[5]; ///< Address for writing
// 	length_t address_width; ///< Width of the address (3, 4, or 5 bytes)
// } radio_t;

void RADIO_init(pin_t ce, pin_t csn);

// byte_t RADIO_read_register(radio_t *radio, byte_t reg);

/**
 * @brief Check whether there is a payload available to read
 * @param radio Pointer to the radio structure
 * @return Non-zero value if there is a payload available, otherwise `0`
 */
inline bool_t RADIO_has_payload(void)
{
	return (!NRF24L01_is_rx_empty());
}

inline void RADIO_set_address_tx(const byte_t *addr)
{
	NRF24L01_set_address_tx(addr);
}

inline void RADIO_set_address_rx(pipe_t pipe, const byte_t *addr)
{
	NRF24L01_set_address_rx(pipe, addr);
}

// void RADIO_ready_tx(void);
// void RADIO_ready_rx(void);

/**
 * @brief Read payload from the RX FIFO buffer
 * @param buffer Pointer to the buffer to store payload
 * @param len Length of the buffer (1 to 32 bytes)
 */
bool_t RADIO_read(byte_t *buffer, length_t len);

/**
 * @brief Write payload to the TX FIFO buffer
 * @param buffer Pointer to the buffer
 * @param len Length of the buffer (1 to 32 bytes)
 */
bool_t RADIO_write(const byte_t *buffer, length_t len);

inline void RADIO_debug(void)
{
	NRF24L01_print();
}

#endif // VEMAR_RADIO_H
