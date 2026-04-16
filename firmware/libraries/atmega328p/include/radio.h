#ifndef VEMAR_RADIO_H
#define VEMAR_RADIO_H

#include "nrf24l01.h"

#if !defined(DEFINE_NRF24L01)
#error "Module 'NRF24L01' not defined"
#endif

/**
 * @brief Initialize the radio
 * @param ce Pin of __CE__
 * @param csn Pin of __CSN__
 */
void RADIO_init(pin_t ce, pin_t csn);

/**
 * @brief Set the TX address
 * @param addr TX addres (5 bytes)
 */
inline void RADIO_set_address_tx(const byte_t *addr)
{
	NRF24L01_set_address_tx(addr);
}

/**
 * @brief Set the RX address of a data pipe
 * @param pipe Data pipe whose address to modify
 * @param addr RX address (5 bytes)
 */
inline void RADIO_set_address_rx(pipe_t pipe, const byte_t *addr)
{
	NRF24L01_set_address_rx(pipe, addr);
}

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

/**
 * @brief Display debug information of the radio
 */
inline void RADIO_debug(void)
{
	NRF24L01_print();
}

#endif // VEMAR_RADIO_H

/**
 * @file radio.h
 * @brief Utility function
 * @author Christian Hugon <chriss.hugon@gmail.com>
 */