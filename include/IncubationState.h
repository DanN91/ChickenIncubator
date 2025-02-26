/*
 * IncubationState.h
 *
 * This file defines the IncubationState class, which represents the incubation phase
 * of the incubation cycle. It inherits from the ICycleState interface and implements
 * the Transition method to handle state transitions specific to the incubation phase.
 */

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