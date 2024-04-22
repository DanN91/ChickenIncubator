#include "ICycleState.h"

ICycleState::ICycleState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings)
    : m_context(context)
    , m_hygrotherm(hygrotherm)
    , m_settings(settings)
{
}

ICycleState::~ICycleState()
{
    m_hygrotherm.Unregister();
}

void ICycleState::Handle(const DS3231& clock)
{
    // Read current date and time
    // and compare against the ChangeStateDate & ChangeStateTime
    // to determine if a state change is needed
    DateTM changeDate;
    if (!m_settings.Get(Settings::ChangeStateDate, changeDate))
        return;

    if (!(clock.GetDate() >= changeDate))
        return;

#if SERIAL_DEBUG
    Serial.print(clock.GetTime().GetTimeFormatted());
    Serial.print(" | ");
    Serial.println(clock.GetDate().GetDateFormatted());
#endif // SERIAL_DEBUG

    TimeTM changeTime;
    if (!m_settings.Get(Settings::ChangeStateTime, changeTime))
        return;

    if (!(clock.GetTime() >= changeTime))
        return;

    Transition();
}

bool ICycleState::ChangeStateOn(const DateTM& date, const TimeTM& time)
{
    return m_settings.Set(Settings::ChangeStateDate, date) && m_settings.Set(Settings::ChangeStateTime, time);
}

bool ICycleState::SetHygrothermParameters(const Parameter<float>& temperature, const Parameter<uint8_t>& humidity) const
{
#ifdef SERIAL_DEBUG
    Serial.print("T: ");
    Serial.print(temperature.Min);
    Serial.print(" - ");
    Serial.print(temperature.Max);
    Serial.print(" | H: ");
    Serial.print(humidity.Min);
    Serial.print(" - ");
    Serial.println(humidity.Max);
#endif // SERIAL_DEBUG

    if (!m_hygrotherm.Temperature(temperature) || !m_hygrotherm.Humidity(humidity))
    {
#ifdef SERIAL_DEBUG
        Serial.println("ERROR: Could not set temperature and humidity for Hygrotherm.");
#endif // SERIAL_DEBUG
        return false;
    }

    m_hygrotherm.Register();

    return true;
}