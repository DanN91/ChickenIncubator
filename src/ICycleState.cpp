#include "ICycleState.h"

#include "Utils.h"

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

    SerialDebug(clock.GetTime().GetTimeFormatted() + " | " + clock.GetDate().GetDateFormatted());

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
    SerialDebug("T: " + String(temperature.Min) + " - " + String(temperature.Max) + " | H: " + String(humidity.Min) + " - " + String(humidity.Max));

    if (!m_hygrotherm.Temperature(temperature) || !m_hygrotherm.Humidity(humidity))
    {
        SerialDebug("ERROR: Could not set temperature and humidity for Hygrotherm.");
        return false;
    }

    m_hygrotherm.Register();

    return true;
}