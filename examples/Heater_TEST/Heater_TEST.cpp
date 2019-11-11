#include <Arduino.h>

// Config
#include <Hardware.h>

// Cooler & Humidifier
#include <Switcher.h>

// User Interaction
#include <PushButton.h>

#include "ButtonObserver.h"

Switcher heater(Hardware::HEATER_PIN, true);
PushButton button(Hardware::BUTTON_PIN);

// Test helper class
ButtonObserver observer(heater, button);

void setup()
{
  Serial.begin(115200);

  heater.Initialize();
  button.Initialize();

  observer.Register();

  Serial.println("Initialized");
}

void loop()
{
  button.HandleEvents();
  delay(100);
}
