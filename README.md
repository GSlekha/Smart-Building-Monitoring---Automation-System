# Smart Building Monitoring & Automation System

## Overview
An ESP32-based prototype for environmental monitoring and automatic control of a small building environment.

## Features
- Temperature, humidity and pressure monitoring using BME280
- Ambient light measurement using BH1750
- Live sensor readings on an OLED display
- Automatic fan control based on temperature
- Automatic light control based on ambient light
- Sensor fault detection and recovery logging
- Wi-Fi dashboard hosted by ESP32 in access-point mode

## Hardware Components
- ESP32 development board
- BME280 environmental sensor
- BH1750 light sensor
- OLED display (128x64)
- Two-channel relay module
- 5V DC fan
- USB LED light

## GPIO Connections
| Function | ESP32 GPIO |
|---|---|
| I2C SDA | GPIO 21 |
| I2C SCL | GPIO 22 |
| Fan relay control | GPIO 25 |
| Light relay control | GPIO 26 |

## Automation Logic
- Fan turns ON above 28°C and OFF below 26°C.
- Light turns ON below 20 lux and OFF above 40 lux.

## Testing and Validation
- Environmental sensor readings verified
- OLED display verified
- Fan and light automation tested
- Sensor fault detection and recovery tested
- Wi-Fi dashboard accessed from a phone

## Project Status
Working educational prototype.
