#pragma once

#include <SettingsManager.h>
#include <Hygrotherm.h>
#include <DS3231.h>

#include <ICycleContext.h>
#include <CycleTypes.h>

class ICycleState
{
public:
    ICycleState(ICycleContext& cycle, Hygrotherm& hygrotherm, SettingsManager& settings);
    virtual ~ICycleState();

    void Handle(const DS3231& clock);

    // non-copyable & non-movable
    ICycleState(const ICycleState&) = delete;
    ICycleState& operator=(const ICycleState&) = delete;
    ICycleState(ICycleState&&) = delete;
    ICycleState& operator=(ICycleState&&) = delete;

protected:
    bool ChangeStateOn(const DateTM& date, const TimeTM& time);
    bool SetHygrothermParameters(const Parameter<float>& temperature, const Parameter<uint8_t>& humidity) const;

    ICycleContext& m_context;
    Hygrotherm& m_hygrotherm;
    SettingsManager& m_settings;

private:
    // Interface shared by all states
    virtual void Transition() = 0;
};