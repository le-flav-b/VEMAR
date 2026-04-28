#ifndef VEMAR_UTIL_H
#define VEMAR_UTIL_H

#include "typedef.h"

/**
 * @brief Convert an integer to string
 * @param n Number to convert
 * @param width Minimum width of the string
 * @return Null-terminated string
 * @details
 * The converted string is right justified, if the width of the number is
 * smaller than the specified width then the character space `' '` will be added
 * @warning Undefined behavior if overflow/undeflow occurs
 */
char *UTIL_itoa(int n, length_t width);

/**
 * @brief Divide the number by 10 then convert to string
 * @param n Number to convert
 * @param width Minimum width of the string
 * @return Null-terminated string as a decimal
 * @details
 * For instance: 10 will become "1.0", 6 will become "0.6"
 * @warning Undefined behavior if overflow/underflow occurs
 */
char *UTIL_itoa_decimal(int n, length_t width);

/**
 * @brief Append a character at the end of a string
 * @param str String to be modified
 * @param ch Character to append
 * @return Null-terminated string with `ch` append at the end of `str`
 */
char *UTIL_append(char *str, char ch);

#endif // VEMAR_UTIL_H

/**
 * @file util.h
 * @brief Utility functions
 * @author Christian Hugon <chriss.hugon@gmail.com>
 */
