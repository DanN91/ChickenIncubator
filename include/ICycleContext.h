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