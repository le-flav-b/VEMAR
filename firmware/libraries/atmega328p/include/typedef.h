#ifndef VEMAR_TYPEDEF_H
#define VEMAR_TYPEDEF_H

#include <stdint.h>
#include <util/delay.h>

#ifndef NULL
#define NULL ((void *)0) /**< Null pointer */
#endif

/**
 * @brief Define 8-bit register
 */
typedef unsigned char byte_t;

/**
 * @brief Define the size of buffer.
 * Used for counting bytes in the range of __[0:255]__
 */
typedef unsigned char length_t;

/**
 * @brief Define boolean type
 */
typedef enum
{
	FALSE = 0, /**< Logical false */
	TRUE = 1   /**< Logical true */
} bool_t;

/**
 * @brief Suspend for an interval of time
 * @param ms Time in milliseconds
 */
#define delay(ms) _delay_ms(ms)

#endif // VEMAR_TYPEDEF_H

/**
 * @file typedef.h
 * @brief Type declarations
 * @author Christian HUGON <chriss.hugon@gmail.com>
 * @version 1.0.0
 */
