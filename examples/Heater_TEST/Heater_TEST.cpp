#include <Arduino.h>

// Config
#include <Hardware.h>

// Cooler & Humidifier
#include <Switcher.h>

// User Interaction
#include <PushButton.h>

#include "ButtonObserver.h"

// Switcher switcher(Hardware::COOLER_PIN);
Switcher switcher(Hardware::HEATER_PIN, true);
// Switcher switcher(Hardware::HUMIDIFIER_PIN);
PushButton button(Hardware::BUTTON_PIN);

// Test helper class
ButtonObserver observer(switcher, button);

void setup()
{
  Serial.begin(115200);

  switcher.Initialize();
  button.Initialize();

  observer.Register();

  Serial.println("Initialized");
}

void loop()
{
  button.HandleEvents();
  delay(100);
}
