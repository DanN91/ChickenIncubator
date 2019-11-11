#pragma once

#include <ObserverPattern.h>
#include <PushButtonMasks.h>
#include <Switcher.h>

class ButtonObserver final : public IObserver<ButtonState>
{
public:
    ButtonObserver(Switcher& switcher, IObservable<ButtonState>& button)
        : IObserver(ButtonState::Released, button)
        , m_switch(switcher)
    {
        m_switch.Off();
    }

    // IObserver<ButtonState>
    void OnEvent(ButtonState event) override
    {
        if (event == ButtonState::Released)
        {
            Serial.println("Released");
            Toggle();
        }
    }

private:
    void Toggle()
    {
        m_switch.IsOn() ? m_switch.Off() : m_switch.On();
        Serial.print("Toggle: ");
        Serial.println(m_switch.IsOn() ? "ON" : "OFF");
    }

    Switcher& m_switch;
};