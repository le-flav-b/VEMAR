#ifndef VEMAR_MODULE_H
#define VEMAR_MODULE_H

#include <typedef.h>
#include <i2c.h>
#include <util/packet.h>

#ifdef VEMAR_DEBUG_ENABLED
#include <serial.h>
#define VEMAR_DEBUG(_type, ...) SERIAL_print(_type, __VA_ARGS__)
#else
#define VEMAR_DEBUG(_type, ...)
#endif

//------------------------------------------------------------------------------
// Atmosphere
//------------------------------------------------------------------------------

#define ATMOSPHERE_ADDRESS 0x09 /**< Default address of the ATMOSPHERE module */

/**
 * @brief Initialize the ATMOSPHERE module
 * @return `TRUE` if the module initialization was successful, otherwise `FALSE`
 */
bool_t ATMOSPHERE_init(void);

/**
 * @brief Read data from the ATMOSPHERE module and populate the transmission packet.
 * @param addr I2C address of the target GAS module
 * @param packet Pointer to the packet structure to fill
 * @return `TRUE` if the reading from the GAS module was successful and
 * packet is filled, otherwise `FALSE`
 */
bool_t ATMOSPHERE_fill_packet(uint8_t addr, packet_t *packet);

//------------------------------------------------------------------------------
// Gas
//------------------------------------------------------------------------------

#define GAS_SIZE 14 /**< 2-byte length header + 12 bytes payload */

// Packet offsets
#define IDX_CO2 2
#define IDX_CO 4
#define IDX_NH3 6
#define IDX_NO2 8
#define IDX_O2 10
#define IDX_TEMP 12
#define IDX_STATUS 13

#define CO2_TEMP_OFFSET 44 // subtract from raw TEMP byte to get °C

#define GAS_ADDRESS 0x0A /**< Default address of the GAS module */

// STATUS bits
#define STATUS_CO2_VALID 0x01
#define STATUS_CO2_PREHEATING 0x02
#define STATUS_CO2_RX_SEEN 0x04
#define STATUS_CO2_FRAME_SEEN 0x08
#define STATUS_CO2_UART_ERR 0x10
#define STATUS_CO2_RX_EDGE 0x20
#define STATUS_CO2_CMD_SENT 0x40

/**
 * @brief Read data from the GAS module and populate the transmission packet.
 * @param addr I2C address of the target GAS module
 * @param packet Pointer to the packet structure to fill
 * @return `TRUE` if the reading from the GAS module was successful and
 * packet is filled, otherwise `FALSE`
 */
bool_t GAS_fill_packet(uint8_t addr, packet_t *packet);


//------------------------------------------------------------------------------
// Geiger counter
//------------------------------------------------------------------------------

#define GEIGER_ADDRESS 0x28 /**< Default address of the GEIGER module */

/**
 * @brief Read data from the GEIGER module and populate the transmission packet.
 * @param addr I2C address of the target GAS module
 * @param packet Pointer to the packet structure to fill
 * @return `TRUE` if the reading from the GAS module was successful and
 * packet is filled, otherwise `FALSE`
 */
bool_t GEIGER_fill_packet(uint8_t addr, packet_t *packet);

#endif // VEMAR_MODULE_H

/**
 * @file module.h
 * @brief Utility functions enabling communication between the car and different
 * modules
 * @author Christian Hugon <chriss.hugon@gmail.com>
 */
