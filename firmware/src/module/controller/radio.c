#include "radio.h"
#include "serial.h"

#define RADIO_DEFAULT_FREQUENCY 42

// #include "spi.h"

// #define RADIO_PAYLOAD_SIZE 32
// #define RADIO_ADDRESS_WIDTH 5
// #define RADIO_SPI_SPEED 10000000 ///< Default SPI speed (10kHz)
// #define RADIO_DELAY 5

// #define RADIO_ACK NRF24L01_SPI_W_TX_PAYLOAD
// #define RADIO_NOACK NRF24L01_SPI_W_TX_PAYLOAD_NOACK

/**
 * Function naming:
 * ce -> RADIO_util_chip_enable
 * csn -> RADIO_util_chip_select
 */

// static inline void RADIO_util_chip_enable(pin_t ce, pin_state_t state)
// {
//     PIN_write(ce, state);
// }

// static inline void RADIO_util_chip_select(pin_t csn, pin_state_t state)
// {
//     PIN_write(csn, state);
//     delay(RADIO_DELAY);
// }

// static inline void RADIO_util_begin(radio_t *radio)
// {
//     RADIO_util_chip_select(radio->csn, PIN_LOW);
// }

// static inline void RADIO_util_end(radio_t *radio)
// {
//     RADIO_util_chip_select(radio->csn, PIN_HIGH);
// }

// static void RADIO_util_powerup(void)
// {
//     delay(RADIO_DELAY);
// }

void RADIO_init(pin_t ce, pin_t csn)
{
    NRF24L01_init(ce, csn);

    // NRF24L01_set_data_rate(NRF24L01_DR_1MBPS);
    NRF24L01_setup(NRF24L01_1MBPS, NRF24L01_N18DBM, 1);
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

    // setRetries(5, 15);
    // setDataRate(RF24_1MBPS);

    // return (retval);
}


// byte_t RADIO_read_register(radio_t *radio, byte_t reg)
// {
//     RADIO_util_begin(radio);
//     SPI_transmit(reg);
//     byte_t retval = SPI_receive();
//     RADIO_util_end(radio);
//     return (retval);
// }

// void RADIO_write_register(radio_t *radio, byte_t reg, byte_t value)
// {
//     RADIO_util_begin(radio);
//     SPI_transmit(NRF24L01_SPI_W_REGISTER | reg);
//     SPI_transmit(value);
//     RADIO_util_end(radio);
// }

void RADIO_ready_rx(void)
{
    NRF24L01_power_up();
    NRF24L01_primary_rx();
    NRF24L01_clear_status();
    NRF24L01_enable();
}

void RADIO_ready_tx(void)
{
    NRF24L01_disable(); // set CE pin LOW

    // NRF24L01_enable();

    _delay_us(130);     // txDelay
    // NRF24L01_flush_tx();
    NRF24L01_primary_tx();
}

// bool_t RADIO_write(const byte_t *payload, length_t len)
// {
//     NRF24L01_write_payload(payload, len);
//     NRF24L01_mode_tx();
//     // NRF24L01_disable();
//     byte_t status = NRF24L01_status();
//     while (BIT_is_clear(status, NRF24L01_TX_DS | NRF24L01_MAX_RT))
//     {
//         status = NRF24L01_status();
//         SERIAL_println(hex, status, 2);
//     }
//     NRF24L01_clear_status();
//     NRF24L01_standby();
//     return (BIT_is_clear(status, NRF24L01_MAX_RT));
// }

/**
 * @todo read
 */
bool_t RADIO_read(byte_t *dst, length_t len)
{
    NRF24L01_mode_rx();
    if (!NRF24L01_is_rx_empty()) {
        NRF24L01_read_payload(dst, len);
        // NRF24L01_standby();
        return (1);
    }
    // NRF24L01_standby();
    return (0);
}

bool_t RADIO_write(const byte_t *payload, length_t len)
{
    NRF24L01_write_payload(payload, len);
SERIAL_println(str, "write payload");

    NRF24L01_mode_tx();
    NRF24L01_disable();

SERIAL_println(bool, NRF24L01_is_tx_empty());
NRF24L01_debug_config();



    // _delay_us(130);
    // _delay_us(10);
    // NRF24L01_disable();
    byte_t status = 0;
    do
    {
        status = NRF24L01_status();
// SERIAL_print(str, "status: 0x"); SERIAL_println(hex, status, 2);
        if (BIT_is_set(status, NRF24L01_MAX_RT))
        {
SERIAL_println(str, "max reached");
            break;
        } // max retransission reached
        delay(100);
    } while (BIT_is_clear(status, NRF24L01_TX_DS)); // wait transmit complete
SERIAL_println(str, "end tx");
    // delay(4);

    // NRF24L01_clear_status();
    // NRF24L01_disable();
    NRF24L01_standby();
    NRF24L01_debug_config();

    if (BIT_is_set(status, NRF24L01_MAX_RT))
    {
        NRF24L01_flush_tx();
        return (0); // failure
    } // flush TX FIFO
    return (1); // success;
}

extern inline void RADIO_debug(void);

// void RADIO_write_payload(radio_t radio)
// {
// 	RADIO_util_begin(radio);
// }

extern inline bool_t RADIO_has_payload(void);

extern inline void RADIO_set_address_tx(const byte_t *);
extern inline void RADIO_set_address_rx(pipe_t, const byte_t *);
