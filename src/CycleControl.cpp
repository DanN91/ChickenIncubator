// 
#include <CycleControl.h>
#include <Hygrotherm.h>

// Days 1-18
Parameter<float> INCUBATION_TEMPERATURE { 37.5, 37.8 };
Parameter<uint8_t> INCUBATION_HUMIDITY { 50, 55 };

// Days 19-21
Parameter<float> LOCKDOWN_TEMPERATURE { 36.1, 37.2 };
Parameter<uint8_t> LOCKDOWN_HUMIDITY { 70, 75 };

const char* STATES[3] = {
  "None",
  "Incubation",
  "Lockdown"
};

CycleControl::CycleControl(IObservable<ButtonState> &button, DS3231& clock, SettingsManager& settings, Hygrotherm& hygrotherm)
        : IObserver(ButtonState::Released | ButtonState::Held, button)
        , m_clock(clock)
        , m_settings(settings)
        , m_hygrotherm(hygrotherm)
{
    // nothing
}

bool CycleControl::Initialize()
{
    // if (!StoreParameters()) // #NOTE:this should be done once, to avoid EEPROM wear off
    //     return false;

    // handle the hardware reset/lost power use case
    if (m_settings.Get(Settings::CycleState, m_state))
    {
        if (!m_settings.Get(Settings::ChangeStateDate, m_date) || !m_settings.Get(Settings::ChangeStateTime, m_time))
        {
#ifdef SERIAL_DEBUG
            Serial.println("Could not read Date and Time for Change State.");
#endif // SERIAL_DEBUG

            return false;
        }

        if (m_state != CycleState::None)
            m_hygrotherm.Register();
    }

    return true;
}

void CycleControl::OnEvent(ButtonState state)
{
    switch (state)
    {
        case ButtonState::Held:
        {
#ifdef SERIAL_DEBUG
            Serial.println("Held");
#endif // SERIAL_DEBUG

            ChangeState(true);
            break;
        }

        case ButtonState::Released:
        {
#ifdef SERIAL_DEBUG
            Serial.println("Released");
#endif // SERIAL_DEBUG

            ChangeState();
#ifdef SERIAL_DEBUG
            PrintParameters();
#endif // SERIAL_DEBUG
            break;
        }

        default:
            return;
    }
}

void CycleControl::Handle()
{
    if (m_state == CycleState::None || !m_time || !m_date)
        return;

    // Read current date and time
    // and compare against the ChangeStateDate & ChangeStateTime
    // to determine if a state change is needed
    DateTM date = m_clock.GetDate();
    if (!(date >= m_date))
        return;

#if TEST_CYCLE
    Serial.print(m_clock.GetTime().GetTimeFormatted());
    Serial.print(" | ");
    Serial.println(m_clock.GetDate().GetDateFormatted());
#endif // TEST_CYCLE

    TimeTM time = m_clock.GetTime();
    if (!(time >= m_time))
        return;

#if SERIAL_DEBUG
    Serial.println("* ChangeState Time & Date *");
    Serial.print(m_time.GetTimeFormatted());
    Serial.print(" | ");
    Serial.println(m_date.GetDateFormatted());
#endif // SERIAL_DEBUG

    ChangeState();
}

#ifdef SERIAL_DEBUG
void CycleControl::PrintParameters()
{
    Serial.println("* Cycle Parameters *");

    TimeTM changeStateTime;
    if (!m_settings.Get(Settings::ChangeStateTime, changeStateTime))
    {
        Serial.println("Error reading change time.");
    }

    DateTM changeStateDate;
    if (!m_settings.Get(Settings::ChangeStateDate, changeStateDate))
    {
        Serial.println("Error reading change date.");
    }

    CycleState state;
    if (!m_settings.Get(Settings::CycleState, state))
    {
        Serial.println("Error reading cycle state.");
    }

    Serial.print("State: ");
    Serial.print(STATES[static_cast<uint8_t>(state)]);
    Serial.print(" | ");
    Serial.print(changeStateDate.GetDateFormatted());
    Serial.print(" @ ");
    Serial.println(changeStateTime.GetTimeFormatted());

    /* Temperature & Humidity */
    Parameter<float> temperature;
    if (!m_settings.Get(Settings::IncubationTemperature, temperature))
    {
        Serial.println("Error reading temperature.");
    }

    Parameter<uint8_t> humidity;
    if (!m_settings.Get(Settings::IncubationHumidity, humidity))
    {
        Serial.println("Error reading humidity.");
    }

    Serial.print("Incubation T: ");
    Serial.print(temperature.Min);
    Serial.print(" - ");
    Serial.print(temperature.Max);
    Serial.print(" | H: ");
    Serial.print(humidity.Min);
    Serial.print(" - ");
    Serial.println(humidity.Max);

    if (!m_settings.Get(Settings::LockdownTemperature, temperature))
    {
        Serial.println("Error reading temperature.");
    }

    if (!m_settings.Get(Settings::LockdownHumidity, humidity))
    {
        Serial.println("Error reading humidity.");
    }

    Serial.print("Lockdown T: ");
    Serial.print(temperature.Min);
    Serial.print(" - ");
    Serial.print(temperature.Max);
    Serial.print(" | H: ");
    Serial.print(humidity.Min);
    Serial.print(" - ");
    Serial.println(humidity.Max);

    Serial.println("***");
}
#endif // SERIAL_DEBUG

bool CycleControl::StoreParameters()
{
    return (m_settings.Set(Settings::IncubationTemperature, INCUBATION_TEMPERATURE)
        && m_settings.Set(Settings::IncubationHumidity, INCUBATION_HUMIDITY)
        && m_settings.Set(Settings::LockdownTemperature, LOCKDOWN_TEMPERATURE)
        && m_settings.Set(Settings::LockdownHumidity, LOCKDOWN_HUMIDITY));
}

void CycleControl::ChangeState(bool reset)
{
    if (reset)
    {
        Reset();
    }

#if SERIAL_DEBUG
    Serial.print("Change State: ");
    Serial.print(STATES[static_cast<uint8_t>(m_state)]);
    Serial.print(" -> ");
    Serial.println(STATES[(static_cast<uint8_t>(m_state) + 1) % 3]);
#endif // SERIAL_DEBUG

    // None [ResetButton] -> Incubation [Time expires] -> Lockdown [Time expires] -> None
    switch (m_state)
    {
        case CycleState::None:
        {
            m_state = CycleState::Incubation;

            m_date = m_clock.GetDate();
#ifndef TEST_CYCLE
            m_date.AddDays(18);
#endif // TEST_CYCLE
            m_settings.Set(Settings::ChangeStateDate, m_date);

            m_time = m_clock.GetTime();
#ifdef TEST_CYCLE
            m_time.SetMinute(m_time.GetMinute() + 1);
#endif // TEST_CYCLE
            m_settings.Set(Settings::ChangeStateTime, m_time);

#ifdef SERIAL_DEBUG
            Serial.println("* Incubation Date & Time *");
            Serial.print(m_date.GetDateFormatted());
            Serial.print(" | ");
            Serial.println(m_time.GetTimeFormatted());
#endif // SERIAL_DEBUG

            break;
        }

        case CycleState::Incubation:
        {
            m_state = CycleState::Lockdown;

            // add 3 more days to current date
            m_date = m_clock.GetDate();
#ifndef TEST_CYCLE
            m_date.AddDays(3);
#endif // TEST_CYCLE
            m_settings.Set(Settings::ChangeStateDate, m_date);

#ifdef TEST_CYCLE
            m_time = m_clock.GetTime();
            m_time.SetMinute(m_time.GetMinute() + 1);
            m_settings.Set(Settings::ChangeStateTime, m_time);
#endif // TEST_CYCLE

#if SERIAL_DEBUG
            Serial.println("* Lockdown Date & Time *");
            Serial.print(m_date.GetDateFormatted());
            Serial.print(" | ");
            Serial.println(m_time.GetTimeFormatted());
#endif // SERIAL_DEBUG

            break;
        }

        case CycleState::Lockdown:
        {
            Reset();
            return;
        }

        default:
            return;
    }

    m_settings.Set(Settings::CycleState, static_cast<uint8_t>(m_state));
    SetHygrothermParameters(m_state);
    m_hygrotherm.Register();
}

void CycleControl::ClearCycle()
{
    // 0 all parameters
    m_state = CycleState::None;
    m_settings.Clear(Settings::CycleState, sizeof(m_state));
    m_settings.Clear(Settings::ChangeStateDate, sizeof(DateTM));
    m_settings.Clear(Settings::ChangeStateTime, sizeof(TimeTM));
}

bool CycleControl::SetHygrothermParameters(CycleState state)
{
    Parameter<float> temperature;
    Parameter<uint8_t> humidity;

    switch (state)
    {
        case CycleState::Incubation:
        {
            if (!m_settings.Get(Settings::IncubationTemperature, temperature) || !m_settings.Get(Settings::IncubationHumidity, humidity))
                return false;
        }

        case CycleState::Lockdown:
        {
            if (!m_settings.Get(Settings::IncubationTemperature, temperature) || !m_settings.Get(Settings::IncubationHumidity, humidity))
                return false;
        }

        default:
            return true;
    }

    return m_hygrotherm.Temperature(temperature) && m_hygrotherm.Humidity(humidity);
}

void CycleControl::Reset()
{
    m_hygrotherm.Unregister();
    ClearCycle();
}