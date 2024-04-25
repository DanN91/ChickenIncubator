// 
#include <CycleControl.h>
#include <Hygrotherm.h>

#include <IdleState.h>
#include <IncubationState.h>
#include <LockdownState.h>

// Days 1-18
Parameter<float> INCUBATION_TEMPERATURE { 37.5, 37.8 };
Parameter<uint8_t> INCUBATION_HUMIDITY { 50, 55 };

// Days 19-21
Parameter<float> LOCKDOWN_TEMPERATURE { 36.1, 37.2 };
Parameter<uint8_t> LOCKDOWN_HUMIDITY { 70, 75 };

CycleControl::CycleControl(IObservable<ButtonState> &button, DS3231& clock, SettingsManager& settings, Hygrotherm& hygrotherm)
        : IObserver(ButtonState::Held | ButtonState::Released, button)
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
    CycleState state = CycleState::Idle;
    if (!m_settings.Get(Settings::CycleState, state))
        return false;

    DateTM changeDate;
    TimeTM changeTime;
    if (!m_settings.Get(Settings::ChangeStateDate, changeDate) || !m_settings.Get(Settings::ChangeStateTime, changeTime))
    {
#ifdef SERIAL_DEBUG
        Serial.println("Could not read Date and Time for Change State.");
#endif // SERIAL_DEBUG

        return false;
    }

    // restore last state, keep the change date and time intact
    if (changeDate && changeTime)
    {
        ChangeState(state, true);
    }

    return true;
}

void CycleControl::OnEvent(ButtonState state)
{
    if (state == ButtonState::Held)
    {
#ifdef SERIAL_DEBUG
        Serial.println("Held: Cycle Reset");
#endif // SERIAL_DEBUG

        ChangeState(CycleState::Incubation);
    }
    else if (state == ButtonState::Released)
    {
#ifdef SERIAL_DEBUG
        Serial.println("Released: Next state");
#endif // SERIAL_DEBUG

        if (!m_state)
            return;
        
        m_state->Transition();
    }
}

void CycleControl::Handle()
{
    if (!m_state)
        return;

    m_state->Handle(m_clock);
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
    Serial.print(static_cast<uint8_t>(state));
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

void CycleControl::ChangeState(CycleState state, bool doRestore)
{
#if SERIAL_DEBUG
    Serial.print(doRestore ? "Restore" : "Change");
    Serial.print(" State: ");
    Serial.println(static_cast<uint8_t>(state));
#endif // SERIAL_DEBUG

    // Idle [ResetButton] -> Incubation [Time expires] -> Lockdown [Time expires] -> None
    switch (state)
    {
        case CycleState::Idle:
        {
            m_state.Reset(new IdleState(*this, m_hygrotherm, m_settings));
            break;
        }

        case CycleState::Incubation:
        {
            m_state.Reset(new IncubationState(*this, m_hygrotherm, m_settings, doRestore ? nullptr : &m_clock));
            break;
        }

        case CycleState::Lockdown:
        {
            m_state.Reset(new LockdownState(*this, m_hygrotherm, m_settings, doRestore ? nullptr : &m_clock));
            break;
        }

        default:
        {
#ifdef SERIAL_DEBUG
            Serial.println("ERROR: CycleState not handled");
#endif // SERIAL_DEBUG
        }
    }
}
