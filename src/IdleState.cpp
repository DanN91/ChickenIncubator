#include <IdleState.h>
#include <DateTM.h>
#include <TimeTM.h>

IdleState::IdleState(ICycleContext& context, Hygrotherm& hygrotherm, SettingsManager& settings)
    : ICycleState(context, hygrotherm, settings)
{
    m_settings.Set(Settings::CycleState, static_cast<uint8_t>(CycleState::Idle));

    m_settings.Clear(Settings::ChangeStateDate, sizeof(DateTM));
    m_settings.Clear(Settings::ChangeStateTime, sizeof(TimeTM));
}

void IdleState::Transition()
{
    // nowhere
}