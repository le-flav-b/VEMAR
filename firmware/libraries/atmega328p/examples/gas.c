#include "i2c.h"
#include "serial.h"

#include "gas_module.h"

byte_t cmd;
gas_data_t gas_data;
byte_t response;

void setup(void)
{
    SERIAL_init();
    I2C_init(I2C_PS1);
    cmd = GAS_MODULE_CMD_RESET;
    I2C_write(GAS_MODULE_ADDRESS, &cmd, 1);
}

void loop(void)
{
    SERIAL_println(str, "loop");
    // cmd = GAS_MODULE_CMD_LED_ON;
    // I2C_write(GAS_MODULE_ADDRESS, &cmd, 1);
    // delay(1000);

    // cmd = GAS_MODULE_CMD_LED_OFF;
    // I2C_write(GAS_MODULE_ADDRESS, &cmd, 1);
    // delay(1000);

    I2C_read(GAS_MODULE_ADDRESS, gas_data.buffer, 2);
    SERIAL_print(str, "a: ");
    SERIAL_println(uint, gas_data.data.a);
    SERIAL_print(str, "b: ");
    SERIAL_println(uint, gas_data.data.b);
    // I2C_read(GAS_MODULE_ADDRESS, &response, 1);
    delay(1000);
}
