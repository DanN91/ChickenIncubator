/*
  ** Chicken Eggs Hatching Temperature and Humidity **
  To Research: https://www.purinamills.com/chicken-feed/education/detail/hatching-eggs-at-home-a-21-day-guide-for-baby-chicks

  Days 1-18: Temperature @ 37.5 - 37.8 Celsius | Humidity @ 50-55%
  Days 19-21: Temperature @ 36.1 - 37.2 Celsius | Humidity @ 70-75%
*/

#include <Arduino.h>

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

uint16_t GetSettingAddress(Settings setting)
{
  switch (setting)
  {
    case Settings::IncubationTemperature: // 8 bytes
      return 0;

    case Settings::IncubationHumidity: // 2 bytes
      return 8;

    case Settings::LockdownTemperature: // 8 bytes
      return 10;

    case Settings::LockdownHumidity: // 2 bytes
      return 18;

    case Settings::CycleState: // 1 byte
      return 20;

    case Settings::ChangeStateDate: // 3 bytes
      return 21;

    case Settings::ChangeStateTime: // 4 bytes
      return 24;

    default:
      return -1; // max uint16_t
  }
}

AT24C32 storage;
SettingsManager settings(storage, GetSettingAddress);

PushButton button(Hardware::BUTTON_PIN);
Hygrotherm hygrotherm(dhtSensor, heater, heater, humidifier);

CycleControl cycle(button, clock, settings, hygrotherm);

void setup()
{
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif // SERIAL_DEBUG

  storage.Initialize();

  heater.Initialize();
  heater.Initialize();
  humidifier.Initialize();

  dhtSensor.Initialize();
  hygrotherm.Initialize();

  button.Initialize();
  cycle.Initialize();

  cycle.Register(); // observes the button

#ifdef SERIAL_DEBUG
  Serial.println("Initialized");
  cycle.PrintParameters();
#endif // SERIAL_DEBUG
}

void loop()
{
  dhtSensor.HandleEvents();
  button.HandleEvents();

  cycle.Handle();

  delay(300);
}
