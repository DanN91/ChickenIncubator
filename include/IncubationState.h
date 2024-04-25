#pragma once

#include "ICycleState.h"

class IncubationState final : public ICycleState
{
public:
    IncubationState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings, const DS3231* clock);
    virtual ~IncubationState() = default;

    // ICycleState
    void Transition() override;
};