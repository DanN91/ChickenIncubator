#include <IncubationState.h>

#include <Utils.h>

IncubationState::IncubationState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings, const DS3231* clock)
    : ICycleState(context, hygrotherm, settings)
{
    // Hygrotherm Parameters
    Parameter<float> temperature;
    Parameter<uint8_t> humidity;

    if (!m_settings.Get(Settings::IncubationTemperature, temperature) || !m_settings.Get(Settings::IncubationHumidity, humidity))
    {
        SerialDebug("ERROR: Could not read temperature and humidity from Settings.");
        return;
    }

    SetHygrothermParameters(temperature, humidity);

    if (clock)
    {
        auto date = clock->GetDate();
        auto time = clock->GetTime();

#ifndef TEST_CYCLE
        date.AddDays(18);
#endif

#ifdef TEST_CYCLE
        time.SetMinute(time.GetMinute() + 1);
#endif // TEST_CYCLE

        if (!ChangeStateOn(date, time))
        {
            SerialDebug("ERROR: Could not set change state date and time.");
            return;
        }

#ifdef SERIAL_DEBUG
        m_settings.Get(Settings::ChangeStateDate, date);
        m_settings.Get(Settings::ChangeStateTime, time);
#endif // SERIAL_DEBUG

        SerialDebug("State: Incubation | " + date.GetDateFormatted() + " @ " + time.GetTimeFormatted());
    }

    m_settings.Set(Settings::CycleState, static_cast<uint8_t>(CycleState::Incubation));
}

void IncubationState::Transition()
{
    m_context.ChangeState(CycleState::Lockdown);
}
