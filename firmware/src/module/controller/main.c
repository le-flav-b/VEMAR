#include <avr/io.h>
#include <util/delay.h>

#include "common.h"
#include "adc.h"
#include "joystick.h"
#include "uart.h"

int main()
{
    UART_init(UART_BAUDRATE_115200, UART_FORMAT_8N1);
    UART_println("Hello, World!");
    while (1)
    {
        volatile char data = UART_receive();
        UART_transmit(data);
    }
}

// int main()
// {
//     ADC_init(ADC_REFERENCE_AVCC, ADC_PRESCALER_64, ADC_RESULT_10);
//     JOYSTICK_init(JOYSTICK_ANALOG_0, JOYSTICK_ANALOG_1, JOYSTICK_BUTTON_0);
//     joystick_t joy;

// #ifdef ARDUINO_IDE
//     Serial.begin(115200);
// #endif

//     while (1)
//     {
//         JOYSTICK_read(&joy,
//                       JOYSTICK_ANALOG_0,
//                       JOYSTICK_ANALOG_1,
//                       JOYSTICK_BUTTON_0);
// #ifdef ARDUINO_IDE
//         Serial.print("X: ");
//         Serial.println(joy.x);
//         Serial.print("Y: ");
//         Serial.println(joy.y);
//         if (joy.button)
//         {
//             Serial.println("Button pressed");
//         }
//         else
//         {
//             Serial.println("Button not pressed");
//         }
// #endif
//         _delay_ms(1000);
//     }
// }
