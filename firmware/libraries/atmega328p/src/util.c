#include "util.h"

#define STRING_BUFFER_SIZE 16 /**< Size of the buffer */

char g_string_buffer[STRING_BUFFER_SIZE]; /**< Buffer */

/**
 * @brief Reverse the buffer
 * @param len Length of the buffer
 */
static void UTIL_reverse(unsigned char len);

//------------------------------------------------------------------------------
// UTIL_itoa
//------------------------------------------------------------------------------

char *UTIL_itoa(int n, length_t width)
{
    bool_t is_neg = FALSE;

    if (0 > n)
    {
        is_neg = TRUE;
        n = -n;
    } // if is negative

    unsigned char len = 0;

    if (0 == n)
    {
        g_string_buffer[len] = '0';
        ++len;
    }

    while (0 != n)
    {
        char digit = (char)(n % 10);
        n = n / 10;
        g_string_buffer[len] = digit + '0';
        ++len;
    }

    if (is_neg)
    {
        g_string_buffer[len] = '-';
        ++len;
    }

    if (len < width)
    {
        g_string_buffer[len] = ' ';
        ++len;
    }

    g_string_buffer[len] = '\0';
    UTIL_reverse(len);
    return (g_string_buffer);
}

//------------------------------------------------------------------------------
// UTIL_itoa_decimal
//------------------------------------------------------------------------------

char *UTIL_itoa_decimal(int n, length_t width)
{
    bool_t is_neg = FALSE;

    if (0 > n)
    {
        is_neg = TRUE;
        n = -n;
    } // if is negative

    char digit = (char)(n % 10);
    n = n / 10;
    g_string_buffer[0] = digit + '0';
    g_string_buffer[1] = '.';

    unsigned char len = 2;
    do
    {
        digit = (char)(n % 10);
        n = n / 10;
        g_string_buffer[len] = digit + '0';
        ++len;
    } while (0 != n);

    if (is_neg)
    {
        g_string_buffer[len] = '-';
        ++len;
    }

    if (len < width)
    {
        g_string_buffer[len] = ' ';
        ++len;
    }

    g_string_buffer[len] = '\0';
    UTIL_reverse(len);
    return (g_string_buffer);
}

//------------------------------------------------------------------------------
// UTIL_append
//------------------------------------------------------------------------------

char *UTIL_append(char *str, char ch)
{
    unsigned char len = 0;
    while ('\0' != str[len])
    {
        ++len;
    }
    str[len] = ch;
    str[len + 1] = '\0';
    return (str);
}

//------------------------------------------------------------------------------
// UTIL_reserse
//------------------------------------------------------------------------------

void UTIL_reverse(unsigned char len)
{
    unsigned char beg = 0;
    unsigned char end = len - 1;
    while (beg < end)
    {
        char tmp = g_string_buffer[beg];
        g_string_buffer[beg] = g_string_buffer[end];
        g_string_buffer[end] = tmp;
        ++beg;
        --end;
    }
}
