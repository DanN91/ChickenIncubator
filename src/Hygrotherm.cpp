#include "Hygrotherm.h"

#include <DHT22Sensor.h>

Hygrotherm::Hygrotherm(DHT22Sensor& sensor, Switcher& cooler, Switcher& heater, Switcher& humidifier)
    : IObserver(static_cast<DHT22Mask>(DHT22Mask::Temperature | DHT22Mask::Humidity), sensor)
    , m_sensor(sensor)
    , m_cooler(cooler)
    , m_heater(heater)
    , m_humidifier(humidifier)
{
}

void Hygrotherm::Initialize()
{
    m_cooler.Off();
    m_heater.Off();
    m_humidifier.Off();
}

bool Hygrotherm::Temperature(const Parameter<float>& range)
{
    if (range.Min < DHT22Sensor::DHT22_MIN_TEMPERATURE || range.Max > DHT22Sensor::DHT22_MAX_TEMPERATURE)
        return false;

    m_temperature = range;

    return true;
}

bool Hygrotherm::Humidity(const Parameter<uint8_t>& range)
{
    if (range.Min < DHT22Sensor::DHT22_MIN_HUMIDITY || range.Max > DHT22Sensor::DHT22_MAX_HUMIDITY)
        return false;

    m_humidity = range;

    return true;
}

void Hygrotherm::OnEvent(DHT22Mask parameter)
{
    switch (parameter)
    {
        case DHT22Mask::Temperature:
        {
            const auto value = m_sensor.Temperature();

#ifdef SERIAL_DEBUG
            Serial.print("T: ");
            Serial.println(value);
#endif // SERIAL_DEBUG

            if (value < m_temperature.Min)
            {
                m_heater.On();
                m_cooler.On();
            }
            else if (value > m_temperature.Max)
            {
                m_heater.Off();
                m_cooler.On(); // bring the temperature down
            }
            else // in range: min < value < max
            {
                m_cooler.Off();
            }

            break;
        }

        case DHT22Mask::Humidity:
        {
            const auto value = m_sensor.Humidity();

#ifdef SERIAL_DEBUG
            Serial.print("H: ");
            Serial.println(value);
#endif // SERIAL_DEBUG

            if (value < m_humidity.Min)
            {
                m_humidifier.On();
            }
            else if (value > m_humidity.Max)
            {
                m_humidifier.Off();
            }

            break;
        }
    }
}

void Hygrotherm::Unregister()
{
    IObserver::Unregister();

    m_cooler.Off();
    m_heater.Off();
    m_humidifier.Off();

    Serial.print("All switchers off: ");
    Serial.println(m_heater.IsOn());
}