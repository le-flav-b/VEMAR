#ifndef VEMAR_PACKET_H
#define VEMAR_PACKET_H

#include <stdint.h>

#define PACKET_SIZE 32       /**< Packet size (32-bytes) */
#define PACKET_ID_CAR 0x01   /**< Packet ID of addressed to CAR */
#define PACKET_ID_ATM 0x02   /**< Packet ID of the atmosphere sensor module */
#define PACKET_ID_GAS 0x03   /**< Packet ID of the gas sensor module */
#define PACKET_ID_LIDAR 0x04 /**< Packet ID of the LiDAR */
#define PACKET_ID_GMC 0x05   /**< Packet ID of the Geiger counter */
#define PACKET_ID_SAVE 0x07
#define PACKET_ID_PING 0xFF  /**< Packet ID for ping */

#define LIDAR_DATA_PER_LINE 5
#define LIDAR_DATA_PER_PACKET 5

typedef struct
{
    uint8_t row;
    uint8_t data[LIDAR_DATA_PER_LINE];
} lidar_data_t;

/**
 * @brief Packet Data Frame
 * @details
 * Each packet type is 32 bytes width.
 *
 * | BYTE | 0  |  1  |  3  |  5  |
 * | HDR  | id | padding |
 * | CAR  | id |
 * | ATM  | id | tmp | hmd | prs |
 * | GAS  | id |
 */
typedef union
{
    uint8_t buffer[PACKET_SIZE]; /**< Data buffer */

    struct
    {
        uint8_t id; /**< ID */
        uint8_t module;
        uint8_t padding[PACKET_SIZE - 2]; /**< Padding */
    } header;                             /**< Header */

    struct
    {
        uint8_t id;                        /**< ID */
        uint8_t module;
        uint8_t lb;                        /**< Left Joystick Button */
        uint8_t rb;                        /**< Right Joystick Button */
        uint16_t pot;                      /**< Potentiometer */
        int16_t lx;                        /**< Left Joystick X */
        int16_t rx;                        /**< Right Joystick X */
        int16_t ly;                        /**< Left Joystick Y */
        int16_t ry;                        /**< Right Joystick Y */
        uint8_t save;                      /**< SD save enabled */
        uint8_t padding[PACKET_SIZE - 15]; /**< Padding */
    } car;                                 /**< Car */

    struct
    {
        uint8_t id;                        /**< ID */
        uint8_t module;
        uint16_t pm25;                     /**< PM2.5 */
        uint16_t pm10;                     /**< PM10 */
        int16_t temperature;               /**< Temperature */
        uint16_t humidity;                 /**< Humidity */
        uint16_t pressure;                 /**< Pressure */
        uint8_t padding[PACKET_SIZE - 12]; /**< Padding */
    } atmosphere;                          /**< Atmosphere sensor module */

    struct
    {
        uint8_t id;                        /**< ID */
        uint8_t module;
        uint16_t header;                   /**< Header */
        uint16_t co2;                      /**< CO2 */
        uint16_t co;                       /**< CO */
        uint16_t nh3;                      /**< NH3 */
        uint16_t no2;                      /**< NO2 */
        uint16_t o2;                       /**< O2 */
        int8_t temp;                       /**< Temperature */
        uint8_t status;                    /**< Status register */
        uint8_t padding[PACKET_SIZE - 16]; /**< Padding */
    } gas;                                 /**< Gas sensor module */

    struct
    {
        uint8_t id;
        uint8_t module;
        lidar_data_t line[LIDAR_DATA_PER_PACKET];
    } lidar;

    struct
    {
        uint8_t id;
        uint8_t module;
        uint16_t total;
        uint16_t delta;
        uint16_t cpm;
        uint8_t padding[PACKET_SIZE - 8];
    } geiger;
} packet_t;

#endif // VEMAR_PACKET_H
