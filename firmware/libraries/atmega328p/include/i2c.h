#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/sleep.h>
#include <util/twi.h>

#include "typedef.h"

#define SLAVE_ADDR 0x09
#define I2C_MASTER
#define I2C_FREQ 100000UL
#define MT_DATA_ACK TW_MT_DATA_ACK
#define MR_DATA_ACK TW_MR_DATA_ACK 
#define MR_DATA_NACK TW_MR_DATA_NACK 

#define I2C_ERR_ARBLOST    (-1)   /* arbitration lost                   */
#define I2C_ERR_BUSERR     (-2)   /* bus error (illegal START/STOP)     */
#define I2C_ERR_NACK       (-3)   /* slave sent NACK                    */
#define I2C_ERR_INVALID_LEN (-4)  /* packet length 0 or > I2C_BUFFER_SIZE */
#define I2C_ERR_TIMEOUT    (-5)   /* hardware flag never set            */


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
};

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
extern int8_t i2c_start(uint8_t addr_rw, bool_t restart);
extern void i2c_stop(void);

/*
 * Data Transfer
 */
extern int8_t i2c_write(uint8_t data);
extern int16_t i2c_read_ack(void);
extern int16_t i2c_read_nack(void);
extern int8_t i2c_write_packet(uint8_t addr, uint8_t *data, uint16_t len);
extern int8_t i2c_read_packet(uint8_t addr, uint8_t *buffer);
extern int32_t i2c_get_read_len(uint8_t addr);

/*
 * Slave responses
 */
extern uint8_t i2c_slave_receive(void);
extern void i2c_slave_transmit(uint8_t byte);
extern void i2c_slave_ack(void);
extern void i2c_slave_nack(void);

#endif // !I2C_H
