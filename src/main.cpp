/*
  ** Chicken Eggs Hatching Temperature and Humidity **
  To Research: https://www.purinamills.com/chicken-feed/education/detail/hatching-eggs-at-home-a-21-day-guide-for-baby-chicks

  Days 1-18: Temperature @ 37.5 - 37.8 Celsius | Humidity @ 50-55%
  Days 19-21: Temperature @ 36.1 - 37.2 Celsius | Humidity @ 70-75%
*/

#include <Arduino.h>

#include <Utils.h>

// Config
#include <Hardware.h>

// Cooler & Humidifier
#include <Switcher.h>

// Temperatur & Humidity
#include <DHT22Sensor.h>
#include <Hygrotherm.h>

// Time & Persistent Storage
#include <DS3231.h>
#include <AT24C32.h>

// Settings
#include <SettingsManager.h>
#include <Settings.h>

// Time tracking
#include <TimeTM.h>
#include <DateTM.h>

// User Interaction
#include <PushButton.h>

#include <CycleControl.h>

DHT22Sensor dhtSensor(Hardware::DHT_PIN, 1, 10, 2);

Switcher heater(Hardware::HEATER_PIN, true);
Switcher humidifier(Hardware::HUMIDIFIER_PIN);
Switcher cooler(Hardware::COOLER_PIN);

DS3231 clock; // #FIXME:Implement as ISubject

AT24C32 storage;
SettingsManager settings(storage, GetSettingAddress);

PushButton button(Hardware::BUTTON_PIN);

void PrintTime()
{
    SerialDebug(clock.GetTime().GetTimeFormatted());
}

Hygrotherm hygrotherm(dhtSensor, cooler, heater, humidifier, PrintTime);

CycleControl cycle(button, clock, settings, hygrotherm);

void setup()
{
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif // SERIAL_DEBUG

  storage.Initialize();

  heater.Initialize();
  cooler.Initialize();
  humidifier.Initialize();

  dhtSensor.Initialize();
  hygrotherm.Initialize();

  button.Initialize();
  cycle.Initialize();

  cycle.Register(); // observes the button
  cycle.PrintParameters();
}

void loop()
{
  dhtSensor.HandleEvents();
  button.HandleEvents();

  cycle.Handle();

  delay(100);
}
