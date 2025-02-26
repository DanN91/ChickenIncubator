/*
 * Cycle Control
 *
 * This class manages the incubation cycle by observing button states and controlling the cycle state.
 * It interacts with the DS3231 clock to keep track of time, the SettingsManager to retrieve and store
 * configuration settings, and the Hygrotherm to monitor and control the temperature and humidity.
 * The class implements the observer pattern to respond to button events and the state pattern to manage
 * different cycle states. It uses the ICycleContext interface to allow state transitions and the IObserver
 * interface to handle button state changes.
 *
 * The CycleControl class transitions between different states such as Idle, Incubation, and Lockdown,
 * each represented by a specific state class (IdleState, IncubationState, LockdownState). These states
 * are managed using the UniquePtr<ICycleState> to ensure proper resource management.
 */

#include <CycleControl.h>
#include <Hygrotherm.h>
#include <IdleState.h>
#include <IncubationState.h>
#include <LockdownState.h>

#include <Utils.h>

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
        SerialDebug("Could not read Date and Time for Change State.");
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
        SerialDebug("Held: Cycle Reset");
        ChangeState(CycleState::Incubation);
    }
    else if (state == ButtonState::Released)
    {
        SerialDebug("Released: Next state");

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

void CycleControl::PrintParameters()
{
    SerialDebug("* Cycle Parameters *");

    TimeTM changeStateTime;
    if (!m_settings.Get(Settings::ChangeStateTime, changeStateTime))
    {
        SerialDebug("Error reading change time."); 
    }

    DateTM changeStateDate;
    if (!m_settings.Get(Settings::ChangeStateDate, changeStateDate))
    {
        SerialDebug("Error reading change date."); 
    }

    CycleState state;
    if (!m_settings.Get(Settings::CycleState, state))
    {
        SerialDebug("Error reading cycle state."); 
    }

    SerialDebug("State: " + String(static_cast<uint8_t>(state)) + " | " + changeStateDate.GetDateFormatted() + " @ " + changeStateTime.GetTimeFormatted()); 

    /* Temperature & Humidity */
    Parameter<float> temperature;
    if (!m_settings.Get(Settings::IncubationTemperature, temperature))
    {
        SerialDebug("Error reading temperature."); 
    }

    Parameter<uint8_t> humidity;
    if (!m_settings.Get(Settings::IncubationHumidity, humidity))
    {
        SerialDebug("Error reading humidity."); 
    }

    SerialDebug("Incubation T: " + String(temperature.Min) + " - " + String(temperature.Max) + " | H: " + String(humidity.Min) + " - " + String(humidity.Max)); 

    if (!m_settings.Get(Settings::LockdownTemperature, temperature))
    {
        SerialDebug("Error reading temperature."); 
    }

    if (!m_settings.Get(Settings::LockdownHumidity, humidity))
    {
        SerialDebug("Error reading humidity."); 
    }

    SerialDebug("Lockdown T: " + String(temperature.Min) + " - " + String(temperature.Max) + " | H: " + String(humidity.Min) + " - " + String(humidity.Max)); 

    SerialDebug("***"); 
}

bool CycleControl::StoreParameters()
{
    return (m_settings.Set(Settings::IncubationTemperature, INCUBATION_TEMPERATURE)
        && m_settings.Set(Settings::IncubationHumidity, INCUBATION_HUMIDITY)
        && m_settings.Set(Settings::LockdownTemperature, LOCKDOWN_TEMPERATURE)
        && m_settings.Set(Settings::LockdownHumidity, LOCKDOWN_HUMIDITY));
}

void CycleControl::ChangeState(CycleState state, bool doRestore)
{
    SerialDebug((doRestore ? "Restore" : "Change") + String(" State: ") + String(static_cast<uint8_t>(state))); 

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
            SerialDebug("ERROR: CycleState not handled"); 
        }
    }
}
