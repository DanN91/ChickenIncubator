#include "Hygrotherm.h"

#include <DHT22Sensor.h>

Hygrotherm::Hygrotherm(DHT22Sensor& sensor, Switcher& cooler, Switcher& heater, Switcher& humidifier, ShowTime printTime)
    : IObserver(static_cast<DHT22Mask>(DHT22Mask::Temperature | DHT22Mask::Humidity), sensor)
    , m_sensor(sensor)
    , m_cooler(cooler)
    , m_heater(heater)
    , m_humidifier(humidifier)
    , m_showTime(printTime)
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
    const auto temperature = m_sensor.Temperature();
    const auto humidity = m_sensor.Humidity();

    // error
    if (temperature == 0.0 || humidity == 0.0)
    {
        m_heater.Off();
        m_cooler.Off();
        m_humidifier.Off();

        return;
    }

    // Temperature
    const float TEMPERATURE_THRESHOLD = 0.2;
    // temperature in range
    if (m_temperature.Min < temperature && temperature <= m_temperature.Max)
    {
        m_heater.Off();
        m_cooler.Off();
    }
    // decrease temperature
    else if (m_temperature.Max < temperature)
    {
        m_heater.Off();
        m_cooler.On();
    }
    // increase temperature
    else if (temperature < (m_temperature.Min - TEMPERATURE_THRESHOLD))
    {
        m_heater.On();
        m_cooler.On();
    }

    // humidity in range
    const uint8_t midRange = (m_humidity.Min + m_humidity.Max) / 2;
    if (midRange <= humidity && humidity <= m_humidity.Max)
    {
        m_humidifier.Off();
        if (!m_heater.IsOn())
        {
            m_cooler.Off();
        }
    }
    // increase humidity
    else if (humidity < m_humidity.Min)
    {
        m_humidifier.On();
    }
    // decrease humidity
    else if (humidity > m_humidity.Max)
    {
        m_cooler.On();
        m_humidifier.Off();
    }

#ifdef SERIAL_DEBUG
    if (m_showTime)
    {
        Serial.print("[ ");
        m_showTime();
        Serial.print(" ] ");
    }

    Serial.print("T: ");
    Serial.print(temperature);
    if (parameter == DHT22Mask::Temperature)
    {
        Serial.print(" * ");
    }

    Serial.print(" | H: ");
    Serial.print(humidity);
    if (parameter == DHT22Mask::Humidity)
    {
        Serial.print(" * ");
    }

    Serial.print(" ~ [");
    Serial.print("H: ");
    Serial.print(m_heater.IsOn());
    Serial.print(" | ");
    Serial.print("F: ");
    Serial.print(m_cooler.IsOn());
    Serial.print(" | ");
    Serial.print("HUM: ");
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