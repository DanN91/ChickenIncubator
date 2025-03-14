/*
 * Hygrotherm.h
 *
 * This file defines the Hygrotherm class, which is responsible for monitoring and
 * controlling the temperature and humidity during the incubation cycle. It uses
 * sensors and switchers to maintain the desired environmental conditions.
 */

#pragma once

#include <Arduino.h>

#include <ObserverPattern.h>
#include <Switcher.h>

#include <DHT22Sensor.h>

typedef void (*ShowTime)();

template<typename T>
struct Parameter
{
    T Min = T();
    T Max = T();

    Parameter() = default;
    Parameter(T minValue, T maxValue)
        : Min(minValue), Max(maxValue) {}

    bool operator==(const Parameter& other)
    {
        return Min == other.Min && Max == other.Max;
    }
};

class Hygrotherm final : public IObserver<DHT22Mask>
{
public:
    Hygrotherm(DHT22Sensor& sensor, Switcher& cooler, Switcher& heater, Switcher& humidifier, ShowTime printTime = nullptr);
    ~Hygrotherm() = default;

    void Initialize();

    bool Temperature(const Parameter<float>& range);
    bool Humidity(const Parameter<uint8_t>& range);

    // IObserver
    void OnEvent(DHT22Mask parameter) override;
    void Unregister() override;

private:
    Parameter<float> m_temperature = { 0, 0 };
    Parameter<uint8_t> m_humidity = { 0, 0 };

    DHT22Sensor& m_sensor;

    Switcher& m_cooler;
    Switcher& m_heater;
    Switcher& m_humidifier;
    ShowTime m_showTime = nullptr;
};
