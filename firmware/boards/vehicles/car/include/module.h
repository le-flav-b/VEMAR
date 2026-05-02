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

#define GAS_SIZE 12 /**< 2-byte length header + 10 bytes payload */

// Payload offsets (length header stripped by i2c_read_packet)
#define IDX_CO2 0
#define IDX_CO 2
#define IDX_NH3 4
#define IDX_NO2 6
#define IDX_O2 8

#define GAS_ADDRESS 0x0A /**< Default address of the GAS module */

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
