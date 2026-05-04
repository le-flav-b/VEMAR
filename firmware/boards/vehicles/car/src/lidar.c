#include "module.h"

/**
 * @brief Combine 2 bytes into 16-bit value
 * @param _high High byte
 * @param _low Low byte
 */

# define U8HL_TO_U16BIT(_high, _low) ((uint16_t)((_high) << 8) | (_low))

#define LIDAR_CMD_SERVO_A_L1 0x10
#define LIDAR_CMD_SERVO_A_L2 0x11
#define LIDAR_CMD_SERVO_A_R1 0x12
#define LIDAR_CMD_SERVO_A_R2 0x13
#define LIDAR_CMD_SERVO_B_L1 0x14
#define LIDAR_CMD_SERVO_B_L2 0x15
#define LIDAR_CMD_SERVO_B_R1 0x16
#define LIDAR_CMD_SERVO_B_R2 0x17
#define LIDAR_CMD_LIDAR_ON 0x20
#define LIDAR_CMD_LIDAR_OFF 0x21

//------------------------------------------------------------------------------
// Liar
//------------------------------------------------------------------------------

bool_t LIDAR_fill_packet(uint8_t addr, packet_t *packet)
{
    uint8_t buffer[I2C_BUFFER_SIZE] = {0};

    if (i2c_read_packet(addr, buffer))
    {
        return (FALSE);
    }

    packet->distance.id = PACKET_ID_LIDAR;
    packet->distance.up = U8HL_TO_U16BIT(buffer[4], buffer[5]);
    packet->distance.down = U8HL_TO_U16BIT(buffer[0], buffer[1]);
    packet->distance.left = U8HL_TO_U16BIT(buffer[2], buffer[3]);
    packet->distance.right = U8HL_TO_U16BIT(buffer[6], buffer[7]);

    VEMAR_DEBUG(str, "== LIDAR Readings ==\r\nUp: ");
    VEMAR_DEBUG(int, packet->distance.up);
    VEMAR_DEBUG(str, "\r\nDown: ");
    VEMAR_DEBUG(int, packet->distance.down);
    VEMAR_DEBUG(str, "\r\nLeft: ");
    VEMAR_DEBUG(int, packet->distance.left);
    VEMAR_DEBUG(str, "\r\nRight: ");
    VEMAR_DEBUG(int, packet->distance.right);
    VEMAR_DEBUG(str, "\r\n");

    return (TRUE);
}

static int8_t _lidar_send_cmd(uint8_t cmd)
{
    int8_t err = i2c_start((LIDAR_ADDRESS << 1), FALSE);
    if (!err) err = i2c_write(cmd);
    i2c_stop();
    return err;
}

void lidar_toggle(void)
{
    static bool_t lidar_on = 0;
    if (lidar_on) _lidar_send_cmd(LIDAR_CMD_LIDAR_OFF);
    else _lidar_send_cmd(LIDAR_CMD_LIDAR_ON);
    lidar_on = !lidar_on;
}

void lidar_mv_a_l1(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_A_L1); }
void lidar_mv_a_l2(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_A_L2); }
void lidar_mv_a_r1(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_A_R1); }
void lidar_mv_a_r2(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_A_R2); }
void lidar_mv_b_l1(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_B_L1); }
void lidar_mv_b_l2(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_B_L2); }
void lidar_mv_b_r1(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_B_R1); }
void lidar_mv_b_r2(void) { _lidar_send_cmd(LIDAR_CMD_SERVO_B_R2); }
