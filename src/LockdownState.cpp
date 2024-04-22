#include <LockdownState.h>

LockdownState::LockdownState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings, const DS3231* clock)
    : ICycleState(context, hygrotherm, settings)
{
    // Hygrotherm Parameters
    Parameter<float> temperature;
    Parameter<uint8_t> humidity;

    if (!m_settings.Get(Settings::LockdownTemperature, temperature) || !m_settings.Get(Settings::LockdownHumidity, humidity))
    {
#ifdef SERIAL_DEBUG
        Serial.println("ERROR: Could not read temperature and humidity from Settings.");
#endif // SERIAL_DEBUG
        return;
    }

    SetHygrothermParameters(temperature, humidity);

    if (clock)
    {
        auto date = clock->GetDate();
        auto time = clock->GetTime();

#ifndef TEST_CYCLE
        date.AddDays(3);
#endif

#ifdef TEST_CYCLE
        time.SetMinute(time.GetMinute() + 1);
#endif // TEST_CYCLE

        if (!ChangeStateOn(date, time))
        {
#ifdef SERIAL_DEBUG
            Serial.println("ERROR: Could not set change state date and time.");
#endif // SERIAL_DEBUG
            return;
        }

#if SERIAL_DEBUG
        m_settings.Get(Settings::ChangeStateDate, date);
        m_settings.Get(Settings::ChangeStateTime, time);

        Serial.print("State: Lockdown | ");
        Serial.print(date.GetDateFormatted());
        Serial.print(" @ ");
        Serial.println(time.GetTimeFormatted());
#endif // SERIAL_DEBUG
    }

    m_settings.Set(Settings::CycleState, static_cast<uint8_t>(CycleState::Lockdown));
}

void LockdownState::Transition()
{
    m_context.ChangeState(CycleState::Idle);
}