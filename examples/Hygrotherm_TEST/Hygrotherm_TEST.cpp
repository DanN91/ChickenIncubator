#include <Arduino.h>

#include <Hardware.h>

// Temperatur & Humidity
#include <DHT22Sensor.h>
#include <Hygrotherm.h>
#include <Switcher.h>

DHT22Sensor dhtSensor(Hardware::DHT_PIN, 1, 10);

Switcher heater(Hardware::HEATER_PIN, true);
Switcher cooler(Hardware::COOLER_PIN);
Switcher humidifier(Hardware::HUMIDIFIER_PIN);

Hygrotherm hygrotherm(dhtSensor, heater, cooler, humidifier);

void setup()
{
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif // SERIAL_DEBUG

  heater.Initialize();
  cooler.Initialize();
  humidifier.Initialize();

  dhtSensor.Initialize();
  hygrotherm.Initialize();

  hygrotherm.Temperature({ 23.0, 25.0 });
  hygrotherm.Humidity({ 65, 70 });

  hygrotherm.Register();

#ifdef SERIAL_DEBUG
  Serial.println("Initialized");
#endif // SERIAL_DEBUG
}

void loop()
{
  dhtSensor.HandleEvents();

  delay(100);
}
