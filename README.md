Technical Documentation: Project VEMAR
======================================
Vehicle for Exploration, Mapping & Air-quality Reconnaissance
-------------------------------------------------------------

**VEMAR** is a modular platform designed for environmetal monitoring and spatial mapping.
By combining real-time locomotion control with a verstile *plug-and-play* sensor architecture, VEMAR provides a comprehensive solution for atmospheric analysis in diverse environments.

## System Overview

The VEMAR system consists of three primary layers:
* A remote controlled vehicle
* Modular Sensors
* Remote Controller

The architecture is designed for high adaptability, allowing users to reconfigure the hardware loadout without complex software recalibration.

### Mobile Platform
* Locomotion: Multi-directional drive system.
* Storage: Integrated SD card slot for localized data logging (JSON format)
* Connectivity: Real-time telemetry link between the vehicle and the handheld controller.

### Modular Sensor Suite
The system utilizes a unified interface that allows the controller to automatically detect and initialize snensors upon connection.

| Sensor ID | Sensor Category | Parameters Measured                                         |
|:---------:|:---------------:|:-----------------------------------------------------------:|
| 1         | Atmospheric     | Temperature, Humidity, Barometric Pressure, PM2.5, PM10     |
| 2         | Gas Analysis    | O2, CO2, CO, NH3, NO2                                       |
| 3         | Mapping         | LiDAR (Light Detection and Ranging) for 2D spatial modeling |
| 4         | Radiation       | Ionizing radiation levels (Geiger-Müller detection)         |

## Functional Capabilities

### Remote Control

The end-user manages the vehicle's navigation via a dedicated controller.
The control link facilites:
* Manual Drive: Real-time steering.
* Data Visualization: A live feedback loop where sensor readings are streamed directly to the controller's display.

### Plug-and-Play Architecture

VEMAR eliminates the need for manual firmware updates when switching hardware.
* Auto-Detection: When a sensor module is attached, the controller identifies the hardware ID and adjusts the UI to display the relevant metrics.
* Hot-Swapping: Sensors can be exchanged in the field to pivot from air-quality monitoring to radiation detection or mapping.

### Data Management

The system supports dual-stream data handling:
* Live Monitoring: Immediate visaul feedback on the controller screen for rapid assessment.
* Archival Recording: On-demand recording to an onboard **SD Card**. this ensures high-fidelity data retention for post-mission analysis and mapping reconstruction.

> Caution: Removing the SD card before stopping the recording may result data corruption of the sd card.
It is recommended to wait at least 5 seconds after disabling saving before safely removing the SD card.

## Operation Workflow

1. Initialization: Power on the vehicle and controller.
2. Module Integration: Plug in desired sensor modules.
3. Verification: Cycle through sensor module readings by pressing the 'A' button on the controller.
4. Deployment: Navigate VEMAR into the target zone while monitoring live data.
5. Logging: Toggle the `Record` function to save data to the SD card by pressing 'B'.
6. Recovery: Disable data logging and retrieve the SD card for data processing.
