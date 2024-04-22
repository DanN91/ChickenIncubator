#pragma once

#include "ICycleState.h"

class LockdownState final : public ICycleState
{
public:
    LockdownState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings, const DS3231* clock);
    virtual ~LockdownState() = default;

private:
    // ICycleState
    void Transition() override;
};