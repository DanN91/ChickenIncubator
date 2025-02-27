/*
 * Hardware.h
 *
 * This file defines the hardware configuration for the incubation system.
 * It includes pin definitions for various components such as sensors, heaters,
 * coolers, and buttons. The pin configurations are provided for different
 * hardware setups.
 */

#pragma once

#include <Arduino.h>

namespace Hardware
{
#ifdef UNO_CONFIG
    constexpr uint8_t HEATER_PIN = 3;
    constexpr uint8_t HUMIDIFIER_PIN = 4;
    constexpr uint8_t COOLER_PIN = 5;

    constexpr uint8_t DHT_PIN = 6;

    constexpr uint8_t BUTTON_PIN = 7;
#endif // UNO_CONFIG

#ifdef NANO_CONFIG
    // #TODO:CHECK PINS
    constexpr uint8_t HEATER_PIN = 3;
    constexpr uint8_t HUMIDIFIER_PIN = 4;
    constexpr uint8_t COOLER_PIN = 5;

    constexpr uint8_t DHT_PIN = 6;

    constexpr uint8_t BUTTON_PIN = 7;
#endif // NANO_CONFIG

#ifdef ATTINY1614_CONFIG
    constexpr uint8_t DHT_PIN = 4;
    constexpr uint8_t HEATER_PIN = 3;
    constexpr uint8_t COOLER_PIN = 2;
    constexpr uint8_t HUMIDIFIER_PIN = 1;
    constexpr uint8_t BUTTON_PIN = 0;
#endif // ATTINY1614_CONFIG

} // Hardware