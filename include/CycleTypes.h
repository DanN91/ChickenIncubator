#pragma once

#include <Arduino.h>

enum class CycleState : uint8_t
{
    Idle = 0,
    Incubation, // Days 1-18
    Lockdown, // Days 19-21
};

inline CycleState operator +(CycleState lhs, uint8_t value)
{
    return static_cast<CycleState>((static_cast<uint8_t>(lhs) + value) % 3);
}
