#include "radio.h"

#define RADIO_DEFAULT_FREQUENCY 42 /**< Default frequency */

typedef enum
{
    RADIO_MODE_STANDBY,
    RADIO_MODE_RX,
    RADIO_MODE_TX
} radio_mode_t;

radio_mode_t g_mode;

//------------------------------------------------------------------------------
// RADIO_init
//------------------------------------------------------------------------------

void RADIO_init(pin_t ce, pin_t csn)
{
    NRF24L01_init(ce, csn);

    NRF24L01_setup(NRF24L01_1MBPS, NRF24L01_0DBM, 1);
    NRF24L01_set_frequency(RADIO_DEFAULT_FREQUENCY);
    NRF24L01_clear_status();

    // flush buffers
    NRF24L01_flush_rx();
    NRF24L01_flush_tx();

    // Enable pipe 0 and 1
    NRF24L01_set_payload_size(NRF24L01_PIPE_0, 32);
    NRF24L01_set_payload_size(NRF24L01_PIPE_1, 32);

    NRF24L01_set_retransmit(10, 5); // 10 retries with delay of 1500us

    NRF24L01_power_up();
    g_mode = RADIO_MODE_STANDBY;
}

//------------------------------------------------------------------------------
// RADIO_read
//------------------------------------------------------------------------------

bool_t RADIO_read(byte_t *dst, length_t len)
{
    if (RADIO_MODE_RX != g_mode)
    {
        NRF24L01_mode_rx();
    }
    if (NRF24L01_has_payload())
    {
        NRF24L01_read_payload(dst, len);
        // NRF24L01_standby();
        return (TRUE);
    }
    g_mode = RADIO_MODE_RX;
    return (FALSE);
}

//------------------------------------------------------------------------------
// RADIO_write
//------------------------------------------------------------------------------

bool_t RADIO_write(const byte_t *payload, length_t len)
{
    NRF24L01_disable();
    NRF24L01_write_payload(payload, len);

    NRF24L01_mode_tx();
    NRF24L01_disable();

    byte_t status = 0;
    do
    {
        status = NRF24L01_status();
        if (BIT_is_set(status, NRF24L01_MAX_RT))
        {
            break;
        } // max retransission reached
        // delay(100);
    } while (BIT_is_clear(status, NRF24L01_TX_DS)); // wait transmit complete

    NRF24L01_standby();
    g_mode = RADIO_MODE_STANDBY;

    if (BIT_is_set(status, NRF24L01_MAX_RT))
    {
        NRF24L01_flush_tx();
        return (FALSE); // failure
    } // flush TX FIFO
    return (TRUE); // success;
}

extern inline void RADIO_set_address_tx(const byte_t *);
extern inline void RADIO_set_address_rx(pipe_t, const byte_t *);
