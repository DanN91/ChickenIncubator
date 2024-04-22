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

#include <CycleTypes.h>
#include <ICycleContext.h>
#include <ICycleState.h>
#include <UniquePtr.h>


class CycleControl final : public IObserver<ButtonState>, public ICycleContext
{
public:
    CycleControl(IObservable<ButtonState>& button, DS3231& clock, SettingsManager& settings, Hygrotherm& hygrotherm);
    ~CycleControl() = default;

    bool Initialize();

    // IObserver
    void OnEvent(ButtonState state);

    // ICycleContext
    void ChangeState(CycleState state, bool doRestore = false) override;

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

    // read from persistence at initialization
    UniquePtr<ICycleState> m_state = nullptr;

    DS3231& m_clock;
    SettingsManager& m_settings;
    Hygrotherm& m_hygrotherm;
};