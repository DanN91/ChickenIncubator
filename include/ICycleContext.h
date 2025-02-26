/*
 * ICycleContext.h
 *
 * This file defines the ICycleContext interface, which provides the context for
 * the cycle states. It allows states to change the current state of the cycle.
 * All cycle context implementations inherit from this interface.
 */

#pragma once

#include <CycleTypes.h>

class ICycleContext
{
public:
    ICycleContext() = default;
    virtual ~ICycleContext() = default;

    // Interface shared by all states
    virtual void ChangeState(CycleState state, bool doRestore = false) = 0;

    // non-copyable & non-movable
    ICycleContext(const ICycleContext&) = delete;
    ICycleContext& operator=(const ICycleContext&) = delete;
    ICycleContext(ICycleContext&&) = delete;
    ICycleContext& operator=(ICycleContext&&) = delete;
};