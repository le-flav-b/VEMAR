//------------------------------------------------------------------------------
// serial.c
//
// Interact with the terminal
//
// Requirements:
// - Use `screen` program to start serial communication
//------------------------------------------------------------------------------

#include "serial.h"

char name[32];
int nb;

void setup(void)
{
    SERIAL_init();
}

void loop(void)
{
    SERIAL_print(str, "Enter your name: ");
    SERIAL_scan(str, name);
    SERIAL_print(str, "Hello ");
    SERIAL_println(str, name);

    SERIAL_print(str, "Enter a number: ");
    SERIAL_scan(int, &nb);
    SERIAL_print(str, "Your number is ");
    SERIAL_println(int, nb);
}
