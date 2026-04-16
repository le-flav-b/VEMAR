#include "uart.h"
#include "config.h"

//------------------------------------------------------------------------------
// UART_init
//------------------------------------------------------------------------------

void UART_init(uart_fmt_t format, uart_mode_t mode)
{
#ifdef UART_ENABLE_2X
    BIT_set(UCSR0A, BIT(U2X0)); // enable double speed
#endif
    UBRR0H = UART_REG_UBRR0H;
    UBRR0L = UART_REG_UBRR0L;

    UCSR0C = (byte_t)(format); // set format
    UCSR0B = (byte_t)(mode);   // enable RX/TX
}

//------------------------------------------------------------------------------
// UART_transmit
//------------------------------------------------------------------------------

void UART_transmit(byte_t data)
{
    WAIT_UNTIL(UART_is_ready());
    UDR0 = data;
}

//------------------------------------------------------------------------------
// UART_receive
//------------------------------------------------------------------------------

byte_t UART_receive(void)
{
    WAIT_UNTIL(UART_is_rx_complete());
    return UDR0;
}

//------------------------------------------------------------------------------
// UART_flush
//------------------------------------------------------------------------------

void UART_flush(void)
{
    byte_t dummy;

    while (UART_is_rx_complete())
    {
        dummy = UDR0; // read UDR0 register
        (void)dummy;
    } // while RXC0 flag is set
}

//------------------------------------------------------------------------------
// UART_reset
//------------------------------------------------------------------------------

void UART_reset(void)
{
    UART_flush();
    BIT_write(UCSR0A, 0x00, (BIT(U2X0) | BIT(MPCM0)));
    UCSR0B = 0x00;
    UCSR0C = (BIT(UCSZ01) | BIT(UCSZ00));
}

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------

extern inline void UART_enable(uart_mode_t);
extern inline void UART_enable_interrupt(uart_mode_t);

extern inline void UART_disable(uart_mode_t);
extern inline void UART_disable_interrupt(uart_mode_t);

extern inline bool_t UART_is_enabled(uart_mode_t);
extern inline bool_t UART_is_ready(void);
extern inline bool_t UART_is_rx_complete(void);
extern inline bool_t UART_is_tx_complete(void);

extern inline bool_t UART_has_overrun(void);
extern inline bool_t UART_has_frame_error(void);
extern inline bool_t UART_has_parity_error(void);

extern inline uart_fmt_t UART_get_format(void);

extern inline void UART_set_format(uart_fmt_t);
