/*
 * Cycle Control
 *
 * #TODO:Description
*/

#pragma once

#include <DS3231.h>
#include <DateTM.h>
#include <TimeTM.h>
#include <SettingsManager.h>
#include <Hygrotherm.h>

#include <ObserverPattern.h>
#include <PushButtonMasks.h>

enum class CycleState : uint8_t
{
    None = 0,
    Incubation, // Days 1-18
    Lockdown, // Days 19-21
};

inline CycleState operator -(CycleState lhs, uint8_t value)
{
    return static_cast<CycleState>(static_cast<uint8_t>(lhs) - value);
}

class CycleControl final : public IObserver<ButtonState>
{
public:
    CycleControl(IObservable<ButtonState>& button, DS3231& clock, SettingsManager& settings, Hygrotherm& hygrotherm);
    ~CycleControl() = default;

    bool Initialize();

    // IObserver
    void OnEvent(ButtonState state);

    void Handle();

#ifdef SERIAL_DEBUG
    void PrintParameters();
#endif // SERIAL_DEBUG

    // non-copyable & non-movable
    CycleControl(const CycleControl &) = delete;
    CycleControl &operator=(const CycleControl &) = delete;
    CycleControl(CycleControl &&) = delete;
    CycleControl &operator=(CycleControl &&) = delete;

private:
    bool StoreParameters();
    bool SetHygrothermParameters(CycleState state);
    void Reset();

    // changes state based on the current state
    void ChangeState(bool reset = false);
    void ClearParameters();

    // read from persistence at initialization
    CycleState m_state = CycleState::None;
    DateTM m_date;
    TimeTM m_time;

    DS3231& m_clock;
    SettingsManager& m_settings;
    Hygrotherm& m_hygrotherm;
};