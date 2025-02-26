/*
 * LockdownState.h
 *
 * This file defines the LockdownState class, which represents the lockdown phase
 * of the incubation cycle. It inherits from the ICycleState interface and implements
 * the Transition method to handle state transitions specific to the lockdown phase.
 */

#pragma once

#include "ICycleState.h"

class LockdownState final : public ICycleState
{
public:
    LockdownState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings, const DS3231* clock);
    virtual ~LockdownState() = default;

    // ICycleState
    void Transition() override;
};