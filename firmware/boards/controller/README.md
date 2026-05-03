# VEMAR Controller

This directory contains the core source code for the __VEMAR Controller__.

## Prerequisites

### Library Dependencies
Before compiling, ensure the __atmega328p__ library is present in the local environment: [${vemar}/firmware/libraries](../../libraries/atmega328p/).

### Hardware Requirements
* Controller PCB
* FTDI Adapter
* USB Cable

## Getting Started

### Hardware Connection
Connect the FTDI module pins to the corresponding headers on the controller
(DTR, RXD, TXD, VCC, CTS, GND) in that order from left to right.
Then plug the FTDI module into your computer via USB.

### Compilation & Flashing
Navigate to the controller directory and use the provided [Makefile](Makefile) to build and upload the firmware.

```sh
# Navigate to the controller directory
cd <path to controller firmware>

# Compile and upload
make
```

If the command fails, ensure you have the `avr-gcc` toolchain installed and that you have permissions to access the USB port.

## Technical Reference

The VEMAR Controller is the primary interface for the vehicle operation and senor data visualization.
This section outlines the hardware layout, display interface, and operational precedures.

### Hardware Layout

```
                               ANT (Antenna)
                               \|/   PWR (Power)
                   LED   LP     |    ||
   +------------------0-0------+++---++---------------------------+
  /             +-------------------------------------+            \
 /              |                                     |             \
/               |                                     |      * *     \
|       +---+   |                                     |    *  A  *   |
|       |   |   |             TFT Display             |      * *     |
|       | T |   |                                     |              |
|       |   |   |                                     |      * *     |
|       +---+   |                                     |    *  B  *   |
|               +-------------------------------------+      * *     |
|                                                                    |
|       *  *         +---------------------------+         *  *      |
|    *        *      |          POT              |      *        *   |
|   *          *     +---------------------------+     *          *  |
|   *    Jl    *                                       *    Jr    *  |
|    *        *  ___________________++++++____________  *        *   |
|       *  *    /                   ||||||            \    *  *      |
\______________/                     FTDI              \_____________/
```

### Component Descriptions

| ID      | Component     | Function                                             |
|:-------:|:-------------:|:----------------------------------------------------:|
| ANT     | Antenna       | Bi-directional antenna for RF communication.         |
| PWR     | Power Switch  | Hard-switch for system power (ON/Off).               |
| LP      | Power LED     | Solid Red light indicates the system is powered.     |
| LED     | User LED      | Programmable status LED (software-defined).          |
| T       | Toggle Switch | 3-Position switch for communication mode selection.  |
| A / B   | Input Buttons | Digital inputs for UI navigation and triggers        |
| POT     | Slider        | Linear potentiometer for auxiliary analog control.   |
| Jl / Jr | Joysticks     | Dual-axis analog sticks for vehicle movement.        |
| FTDI    | Serial Port   | 6-pin interface for firmware flashing and debugging. |


### Display Interface

The onboard TFT screen provides real-time telemetry and hardware diagnostics.

```
+------------------------------+
| [MODULE]                [ID] | <-- Active Module & Available module IDs
|------------------------------|
|                              |
|        TELEMETRY DATA        | <-- Module-specific information
|                              |
|------------------------------|
| [signal]             [Rx/Tx] | <-- Signal Strength & Communication Status
+------------------------------+
```

* Module ID List: Displays IDs of all discovered modules:
    - 1: Atmospheric
    - 2: Gas
    - 3: LiDAR
    - 4: Geiger Counter
* Signal Strength: Visualized from `0` (Disconnected) to `4` (Maximum Strength).
* Communication Mode:
    - Tx: Transmitting commands to vehicle.
    - Rx: Receiving telemetry from sensors.
    - Rx/Tx: Full-duplex bi-directional communication

### Operating Instructions

Toggle the __Power Switch (PWR)__ to the `ON` position. Ensure the __Power LED (LP)__ illuminates.

The __Toggle Switch (T)__ defines the radio behavior.
> *Note*: Telemetry requiers an active Rx link.

| Position | Mode    | Description                                |
|:--------:|:-------:|:------------------------------------------:|
| Up       | Tx Only | Drive vehicle; no sensor data received.    |
| Middle   | Rx/Tx   | Full control and real-time telemetry.      |
| Down     | Rx Only | Monitor sensors; vehicle control disabled. |

Movement of the vehicle is controlled via the __Left Joystick (Jl)__ and __Right Joystick (Jr)__.
> *Note*: The controller must be in __Tx__ or __Rx/Tx__ mode for movement commands to be precessed by the vehicle.

If multiple sensor modules are detected, press __Button A__ to cycle through the available data screens.
> *Note*: Telemetry updates on-screen only when the controller is in a mode that supports __Rx__.

__Button B__ and the __Slider (POT)__ are currently reserved for future firmware implementations.
