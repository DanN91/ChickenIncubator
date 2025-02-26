# Chicken Incubator

## Overview

The Chicken Incubator project is designed to control the temperature and humidity for hatching chicken eggs. It is based on an Arduino setup and includes sensors and actuators to maintain the optimal conditions for incubation.

## Classes

### Include Folder

- **Hardware.h:** Configuration for the hardware components pins.
- **Switcher.h:** Controls devices like heater, humidifier, and cooler.
- **DHT22Sensor.h:** Reads temperature and humidity from the DHT22 sensor.
- **Hygrotherm.h:** Manages temperature and humidity control.
- **DS3231.h:** Manages real-time clock operations.
- **AT24C32.h:** Handles persistent settings storage operations.
- **SettingsManager.h:** Manages various settings for the incubation process.
- **TimeTM.h & DateTM.h:** Handles time and date operations.
- **PushButton.h:** Manages user interactions through a push button.
- **CycleControl.h:** Controls the incubation cycle based on sensor readings and user interactions using State Machine pattern.

### Main Code File (src/main.cpp)

The main code file includes the initialization and configuration of sensors and devices, manages settings, and controls the incubation cycle. It continuously handles events from sensors and user interactions to maintain the optimal conditions for hatching eggs.

## Usage

1. Upload the code to your Arduino board.
2. Ensure all sensors and devices are correctly connected.
3. The system will automatically manage the incubation process based on the predefined settings.

## License

This project is licensed under the MIT License.
