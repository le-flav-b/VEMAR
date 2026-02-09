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
    SERIAL_print("Enter your name: ", str);
    SERIAL_scan(name, str);
    SERIAL_print("Hello ", str);
    SERIAL_println(name, str);

    SERIAL_print("Enter a number: ", str);
    SERIAL_scan(&nb, int);
    SERIAL_print("Your number is ", str);
    SERIAL_println(nb, int);
}
