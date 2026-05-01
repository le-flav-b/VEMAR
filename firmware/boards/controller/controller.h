#ifndef VEMAR_CONTROLLER_H
#define VEMAR_CONTROLLER_H

#include <joystick.h>
#include <tft.h>
#include <radio.h>
#include <util.h>
#include <util/packet.h>

#ifdef VEMAR_DEBUG_ENABLED
#include "serial.h"
#define CONTROLLER_DEBUG(_type, ...) \
    SERIAL_print(_type, __VA_ARGS__)
#else
#define CONTROLLER_DEBUG(_type, ...)
#endif

// Pins
#define PIN_TOGGLE_UP PIN_PD3
#define PIN_TOGGLE_DOWN PIN_PD4
#define PIN_BUTTON1 PIN_PC0
#define PIN_BUTTON2 PIN_PC1
#define PIN_POTENTIOMETER ADC_CH3
#define PIN_JOY_RX ADC_CH7
#define PIN_JOY_RY ADC_CH6
#define PIN_JOY_RB PIN_PC2
#define PIN_JOY_LX ADC_CH5
#define PIN_JOY_LY ADC_CH4
#define PIN_JOY_LB PIN_PD2
#define PIN_LED PIN_PD5

#define PIN_RADIO_CE PIN_PD7
#define PIN_RADIO_CSN PIN_PD6
#define PIN_TFT_CS PIN_PB2
#define PIN_TFT_DC PIN_PB0
#define PIN_TFT_RST PIN_PB1

#define _ROW_num(x) (18U * x + 6U)

#define COL1 4U
#define COL2 140U
#define COL3 212U
#define COL_MOD 260U
#define COL_RXTX 240U
#define ROW_LABEL _ROW_num(0)
#define ROW1 _ROW_num(1)
#define ROW2 _ROW_num(2)
#define ROW3 _ROW_num(3)
#define ROW4 _ROW_num(4)
#define ROW5 _ROW_num(5)
#define ROW6 _ROW_num(6)
#define ROW7 _ROW_num(7)
#define ROW8 _ROW_num(8)
#define ROW9 _ROW_num(9)
#define ROW10 _ROW_num(10)
#define ROW11 _ROW_num(11)
#define ROW12 _ROW_num(12)
#define ROW13 _ROW_num(13)
#define ROW14 _ROW_num(14)
#define ROW_LAST 222U

/**
 * @brief Define all inputs of the controller
 */
typedef struct
{
    struct
    {
        button_t up;   /**< Toggle Up */
        button_t down; /**< Toggle Down */
    } tgl;             /**< Toggle button */
    button_t btn1;     /**< Button 1 */
    button_t btn2;     /**< Button 2 */
    analog_t pot;      /**< Potentiometer */
    joystick_t jright; /**< Right Joystick */
    joystick_t jleft;  /**< Left Joystick */
    led_t led;         /**< LED */
} controller_t;

/**
 * @brief Configure all inputs
 */
void CONTROLLER_init(void);

/**
 * @brief Setup interruptions
 */
void CONTROLLER_interrupt(void);

/**
 * @brief Display main menu
 */
void CONTROLLER_display_menu(void);
/**
 * @brief Display atmospheric sensor data
 */
void CONTROLLER_display_atmosphere(void);

/**
 * @brief Display gas sensor data
 */
void CONTROLLER_display_gas(void);

/**
 * @brief Display no sensor found
 */
void CONTROLLER_display_none(void);
/**
 * @brief D
 */
void CONTROLLER_display_map(void);

void CONTROLLER_display_radioactivity(void);

/**
 * @brief Read and parse received payload
 */
void CONTROLLER_read(void);

/**
 * @brief Send payload to the car
 */
void CONTROLLER_write(void);

/**
 * @brief Update the atmosphere data on the display
 */
void CONTROLLER_update_atmosphere(void);

void CONTROLLER_update_gas(void);

void CONTROLLER_update_radioactivity(void);

/**
 * @brief Update map
 */
void CONTROLLER_update_map(void);


/**
 * @brief Update connection status on the display
 */
void CONTROLLER_update_connection(void);


#endif // VEMAR_CONTROLLER_H
