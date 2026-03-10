#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/sleep.h>


#define MT_SLA_ACK  0x18
#define MT_DATA_ACK 0x28
#define MR_DATA_NACK 0x58

/*
 * Utils
 */
#define TWI_BAUD (((F_CPU / I2C_FREQ) - 10) / 2)
#define TWBR_VAL (((F_CPU / I2C_FREQ) - 16) / 2)

#define I2C_BUFFER_SIZE 64

struct i2cMessage {
    uint16_t len;
    uint8_t buffer[I2C_BUFFER_SIZE];
    uint16_t current_idx;
}

/*
 * Asserts
*/
#if defined(I2C_MASTER) && defined(I2C_SLAVE)
    #error "Cannot define both i2c master and slave. Edit the makefile"
#endif

#if !defined(I2C_MASTER) && !defined(I2C_SLAVE)
    #error "You need to define either the i2c master or slave in the makefile."
#endif

/*
 * Setup
 */
extern void i2c_init(void);
extern void i2c_init_slave(uint8_t addr);
// extern void i2c_set_callback(void (*func)(void)); // Handle events
extern void i2c_stop_interface(void);

/*
 * Control
 */
extern int8_t i2c_start(uint8_t addr_rw, bool restart);
extern void i2c_stop(void);

/*
 * Data Transfer
 */
extern int8_t i2c_write(uint8_t data);
extern int16_t i2c_read_ack(void);
extern int16_t i2c_read_nack(void);
extern int8_t i2c_write_packet(uint8_t addr, uint8_t *data, size_t len);
extern int8_t i2c_read_packet(uint8_t addr, uint8_t *buffer, size_t len);

/*
 * Slave responses
 */
extern uint8_t i2c_slave_receive(void);
extern void i2c_slave_transmit(uint8_t byte);
extern void i2c_slave_ack(void);
extern void i2c_slave_nack(void);

#endif // !I2C_H
