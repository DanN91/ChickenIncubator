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

            // error
            if (value == 0.0)
            {
                m_heater.Off();
                m_cooler.Off();
                break;
            }

            // temperature in range
            if (value > m_temperature.Min && value < m_temperature.Max)
            {
                m_heater.Off();
                m_cooler.Off();
            }
            // increase temperature
            else if (value < m_temperature.Min)
            {
                m_heater.On();
                m_cooler.On();
            }
            // decrease temperature
            else if (value > m_temperature.Max)
            {
                m_cooler.On();
            }

            break;
        }

        case DHT22Mask::Humidity:
        {
            const auto value = m_sensor.Humidity();

            // error
            if (value == 0)
            {
                m_humidifier.Off();
                break;
            }

            // humidity in range
            if (value > m_humidity.Min && value < m_humidity.Max)
            {
                m_humidifier.Off();
                if (!m_heater.IsOn())
                {
                    m_cooler.Off();
                }
            }
            // increase humidity
            else if (value < m_humidity.Min)
            {
                m_humidifier.On();
            }
            // decrease humidity
            else if (value > m_humidity.Max)
            {
                m_cooler.On();
            }

            break;
        }
    }

#ifdef SERIAL_DEBUG
    Serial.print("T: ");
    Serial.print(m_sensor.Temperature());
    Serial.print(" | H: ");
    Serial.print(m_sensor.Humidity());

    Serial.print(" [");
    Serial.print("Heater: ");
    Serial.print(m_heater.IsOn());
    Serial.print(" | ");
    Serial.print("Cooler: ");
    Serial.print(m_cooler.IsOn());
    Serial.print(" | ");
    Serial.print("Humidifier: ");
    Serial.print(m_humidifier.IsOn());
    Serial.println("]");
#endif // SERIAL_DEBUG
}

void Hygrotherm::Unregister()
{
    IObserver::Unregister();

    m_cooler.Off();
    m_heater.Off();
    m_humidifier.Off();
}