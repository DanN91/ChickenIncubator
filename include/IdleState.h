/*
 * IdleState.h
 *
 * This file defines the IdleState class, which represents the idle phase
 * of the incubation cycle. It inherits from the ICycleState interface and implements
 * the Transition method to handle state transitions specific to the idle phase.
 */

#pragma once

#include "ICycleState.h"

class IdleState final : public ICycleState
{
public:
    IdleState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings);
    virtual ~IdleState() = default;

    // ICycleState
    void Transition() override;
};