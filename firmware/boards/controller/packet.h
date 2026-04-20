#ifndef VEMAR_PACKET_H
#define VEMAR_PACKET_H

#include <stdint.h>

#define PACKET_SIZE 32     /**< Packet size (32-bytes) */
#define PACKET_ID_CAR 0x01 /**< Packet ID of addressed to CAR */
#define PACKET_ID_ATM 0x02 /**< Packet ID from the atmosphere sensor module */
#define PACKET_ID_GAS 0x03 /**< Packet ID from the gas sensor module */

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
    uint8_t data[PACKET_SIZE]; /**< Data buffer */

    struct
    {
        uint8_t id;                       /**< ID */
        uint8_t padding[PACKET_SIZE - 1]; /**< Padding */
    } header;                             /**< Header */

    struct
    {
        uint8_t id;
        uint16_t velocity;
        uint8_t padding[PACKET_SIZE - 4];
    } car;

    struct
    {
        uint8_t id;                       /**< ID */
        uint16_t temperature;             /**< Temperature */
        uint16_t humidity;                /**< Humidity */
        uint16_t pressure;                /**< Pressure */
        uint8_t padding[PACKET_SIZE - 8]; /**< Padding */
    } atmosphere;                         /**< Atmosphere sensor module */

    struct
    {
        uint8_t id;                       /**< ID */
        uint16_t co2;
        uint16_t co;
        uint16_t nh3;
        uint16_t no2;
        uint8_t padding[PACKET_SIZE - 10]; /**< Padding */
    } gas;                                /**< Gas sensor module */
} packet_t;

#endif // VEMAR_PACKET_H
