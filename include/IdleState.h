#pragma once

#include "ICycleState.h"

class IdleState final : public ICycleState
{
public:
    IdleState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings);
    virtual ~IdleState() = default;

private:
    // ICycleState
    void Transition() override;
};