#include <avr/io.h>
#include <util/delay.h>

#include "joystick.h"

int main()
{
    ADC_init(ADC_REFERENCE_AVCC, ADC_PRESCALER_64, ADC_RESULT_10);
    JOYSTICK_init(JOYSTICK_ANALOG_0, JOYSTICK_ANALOG_1, JOYSTICK_BUTTON_0);
    joystick_t joy;

#ifdef ARDUINO_IDE
    Serial.begin(115200);
#endif

    while (1)
    {
        JOYSTICK_read(&joy,
                      JOYSTICK_ANALOG_0,
                      JOYSTICK_ANALOG_1,
                      JOYSTICK_BUTTON_0);
#ifdef ARDUINO_IDE
        Serial.print("X: ");
        Serial.println(joy.x);
        Serial.print("Y: ");
        Serial.println(joy.y);
        if (joy.button)
        {
            Serial.println("Button pressed");
        }
        else
        {
            Serial.println("Button not pressed");
        }
#endif
        _delay_ms(1000);
    }
}
