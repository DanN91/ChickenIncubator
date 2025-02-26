/*
 * Cycle Control
 *
 * This class manages the incubation cycle by observing button states and
 * controlling the cycle state. It interacts with the DS3231 clock to keep track
 * of time, the SettingsManager to retrieve and store configuration settings,
 * and the Hygrotherm to monitor and control the temperature and humidity.
 *
 * The class implements the observer pattern to respond to button events and the
 * state pattern to manage different cycle states. It uses the ICycleContext
 * interface to allow state transitions and the IObserver interface to handle
 * button state changes.
 */

#pragma once

#include <DS3231.h>
#include <DateTM.h>
#include <Hygrotherm.h>
#include <SettingsManager.h>
#include <TimeTM.h>

#include <ObserverPattern.h>
#include <PushButtonMasks.h>

#include <CycleTypes.h>
#include <ICycleContext.h>
#include <ICycleState.h>
#include <UniquePtr.h>

class CycleControl final : public IObserver<ButtonState>, public ICycleContext {
public:
    CycleControl(IObservable<ButtonState>& button, DS3231& clock, SettingsManager& settings, Hygrotherm& hygrotherm);
    ~CycleControl() = default;

    bool Initialize();

    // IObserver
    void OnEvent(ButtonState state);

    // ICycleContext
    void ChangeState(CycleState state, bool doRestore = false) override;

    void Handle();

    void PrintParameters();

    // non-copyable & non-movable
    CycleControl(const CycleControl &) = delete;
    CycleControl &operator=(const CycleControl &) = delete;
    CycleControl(CycleControl &&) = delete;
    CycleControl &operator=(CycleControl &&) = delete;

private:
    bool StoreParameters();

    // read from persistence at initialization
    UniquePtr<ICycleState> m_state = nullptr;

    DS3231& m_clock;
    SettingsManager& m_settings;
    Hygrotherm& m_hygrotherm;
};