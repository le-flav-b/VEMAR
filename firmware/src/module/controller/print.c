#include <limits.h>

#include "print.h"

#define PRINT_BASE_STRING "0123456789ABCDEF"
#define PRINT_BASE_DECIMAL 10
#define PRINT_BASE_HEXADECIMAL 16

static void PRINT_base(unsigned long nbr, byte_t radix)
{
	if (0 == nbr)
	{
		PRINT_char(PRINT_BASE_STRING[0]);
		return;
	}
	byte_t pos = (nbr % radix);
	nbr /= radix;
	if (0 < nbr)
	{
		PRINT_base(nbr, radix);
	}
	PRINT_char(PRINT_BASE_STRING[pos]);
}

void PRINT_int(int nbr)
{
	if (0 > nbr)
	{
		PRINT_char('-');
		PRINT_base(ULONG_MAX - (unsigned long)nbr + 1UL, PRINT_BASE_DECIMAL);
	}
	else
	{
		PRINT_base((unsigned long)(nbr), PRINT_BASE_DECIMAL);
	}
}

void PRINT_uint(unsigned int nbr)
{
	PRINT_base(nbr, PRINT_BASE_DECIMAL);
}

void PRINT_long(long nbr)
{
	if (0 > nbr)
	{
		PRINT_char('-');
		PRINT_base(ULONG_MAX - (unsigned long)nbr + 1UL, PRINT_BASE_DECIMAL);
	}
	else
	{
		PRINT_base((unsigned long)nbr, PRINT_BASE_DECIMAL);
	}
}

void PRINT_ulong(unsigned long nbr)
{
	PRINT_base(nbr, PRINT_BASE_HEXADECIMAL);
}

void PRINT_hex(unsigned long nbr)
{
	PRINT_base(nbr, PRINT_BASE_HEXADECIMAL);
}

void PRINT_string(const char *str)
{
	while ('\0' != *str)
	{
		PRINT_char(*str);
		++str;
	}
}

extern inline void PRINT_init(void);
extern inline void PRINT_char(char ch);
