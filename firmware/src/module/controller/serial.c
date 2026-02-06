#include <limits.h>

#include "serial.h"

#define SERIAL_BASE_STRING "0123456789ABCDEF"
#define SERIAL_BASE_DECIMAL 10
#define SERIAL_BASE_HEXADECIMAL 16

#define SERIAL_BUFFER_SIZE 32

#define ASCII_ESCAPE 27
#define ASCII_DELETE 127
#define ASCII_RETURN 13
#define ASCII_BACKSPACE 8
#define ASCII_CSI 91

char serial_buffer[SERIAL_BUFFER_SIZE];

static void SERIAL_print_base(unsigned long nbr, byte_t radix)
{
    if (0 == nbr)
    {
        UART_transmit((byte_t)SERIAL_BASE_STRING[0]);
        return;
    }
    byte_t pos = (nbr % radix);
    nbr /= radix;
    if (0 < nbr)
    {
        SERIAL_print_base(nbr, radix);
    }
    UART_transmit((byte_t)SERIAL_BASE_STRING[pos]);
}

void SERIAL_init(void)
{
    UART_init(UART_BAUDRATE_115200, UART_8N1, UART_TX | UART_RX);
    // UART_set_baudrate(UART_BAUDRATE_115200);
    // UART_set_format(UART_FORMAT_8N1);
    // UART_enable_transmitter();
    // UART_enable_receiver();
    // UART_enable_double_speed();
}

void SERIAL_print_char(char ch)
{
    UART_transmit((byte_t)ch);
}

void SERIAL_print_int(int nbr)
{
    if (0 > nbr)
    {
        UART_transmit((byte_t)'-');
        SERIAL_print_base(ULONG_MAX - (unsigned long)nbr + 1UL,
                          SERIAL_BASE_DECIMAL);
    }
    else
    {
        SERIAL_print_base((unsigned long)(nbr), SERIAL_BASE_DECIMAL);
    }
}

void SERIAL_print_uint(unsigned int nbr)
{
    SERIAL_print_base(nbr, SERIAL_BASE_DECIMAL);
}

void SERIAL_print_long(long nbr)
{
    if (0 > nbr)
    {
        UART_transmit((byte_t)'-');
        SERIAL_print_base(ULONG_MAX - (unsigned long)nbr + 1UL,
                          SERIAL_BASE_DECIMAL);
    }
    else
    {
        SERIAL_print_base((unsigned long)nbr, SERIAL_BASE_DECIMAL);
    }
}

void SERIAL_print_ulong(unsigned long nbr)
{
    SERIAL_print_base(nbr, SERIAL_BASE_HEXADECIMAL);
}

void SERIAL_print_hex(unsigned long nbr)
{
    SERIAL_print_base(nbr, SERIAL_BASE_HEXADECIMAL);
}

void SERIAL_print_str(const char *str)
{
    while ('\0' != *str)
    {
        UART_transmit((byte_t)(*str));
        ++str;
    }
}

static void SERIAL_scan_buffer(void)
{
    signed char pos = 0;
    bool_t escape = 0;

    while (1)
    {
        serial_buffer[pos] = (char)UART_receive();
        if (ASCII_RETURN == serial_buffer[pos])
        {
            UART_transmit('\r');
            UART_transmit('\n');
            break;
        } // if enter
        else if (ASCII_DELETE == serial_buffer[pos])
        {
            if (0 < pos)
            {
                UART_transmit('\b');
                UART_transmit(' ');
                UART_transmit('\b');
                --pos;
            }
            continue;
        } // if delete
        else if (ASCII_ESCAPE == serial_buffer[pos])
        {
            escape = 1;
            continue;
        } // if escape character
        else if (1 == escape && ASCII_CSI == serial_buffer[pos])
        {
            UART_receive(); // discard character
            escape = 0;
            continue;
        } // if control sequence introducer

        if (pos < SERIAL_BUFFER_SIZE)
        {
            UART_transmit(serial_buffer[pos]);
            ++pos;
        }
    }
    serial_buffer[pos] = '\0';
}

void SERIAL_scan_char(char *ch)
{
    SERIAL_scan_buffer();
    *ch = serial_buffer[0];
}

void SERIAL_scan_str(char *str)
{
    byte_t pos = 0;

    SERIAL_scan_buffer();
    while ('\0' != serial_buffer[pos])
    {
        str[pos] = serial_buffer[pos];
        ++pos;
    }
    str[pos] = '\0';
}

static inline bool_t SERIAL_util_is_whitepace(char ch)
{
    if (' ' == ch ||
        '\t' == ch ||
        '\n' == ch ||
        '\v' == ch ||
        '\f' == ch ||
        '\r' == ch)
    {
        return 1;
    }
    return 0;
}

static long SERIAL_util_atol(const char *str)
{
    while (SERIAL_util_is_whitepace(*str))
    {
        ++str;
    }
    long sign = 1;
    long retval = 0;

    if ('-' == *str)
    {
        sign = -1;
        ++str;
    }
    while ('0' <= *str && '9' >= *str)
    {
        retval = retval * 10L + (*str - '0');
        ++str;
    }

    return (sign * retval);
}

static unsigned long SERIAL_util_atou(const char *str)
{
    while (SERIAL_util_is_whitepace(*str))
    {
        ++str;
    }
    unsigned long retval = 0;

    while ('0' <= *str && '9' >= *str)
    {
        retval = retval * 10L + (*str - '0');
        ++str;
    }

    return (retval);
}

void SERIAL_scan_int(int *nbr)
{
    SERIAL_scan_buffer();
    (*nbr) = (int)SERIAL_util_atol(serial_buffer);
}

void SERIAL_scan_uint(unsigned int *nbr)
{
    SERIAL_scan_buffer();
    (*nbr) = (int)SERIAL_util_atou(serial_buffer);
}

void SERIAL_scan_long(long *nbr)
{
    SERIAL_scan_buffer();
    (*nbr) = SERIAL_util_atol(serial_buffer);
}

void SERIAL_scan_ulong(unsigned long *nbr)
{
    SERIAL_scan_buffer();
    (*nbr) = SERIAL_util_atou(serial_buffer);
}
