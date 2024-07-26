#include "Delay.h"
#include "DhtSensor.h"
#include "HeatingCallback.h"
#include "PidController.h"
#include "SamplingCallback.h"

const int sssr_pin = 2;

SamplingCallback sampling_callback;
HeatingCallback heating_callback(sssr_pin);
DhtSensor dht_sensor;
PidController pid(dht_sensor, 37.8, 2.0);

Delay sampling_delay(2000, &sampling_callback);
Delay heating_delay(2000, &heating_callback);

void setup() {
  pinMode(sssr_pin, OUTPUT);
  sampling_delay.Start();
}

void loop() {
  sampling_delay.Update();

  if (!sampling_delay.active_) {
    float output = pid.Calculate();

    if (output > 0.0) {
      heating_callback.StartHeating(output);
      heating_delay.Start();
    } else {
      heating_callback.is_heating_ = false;
      digitalWrite(sssr_pin, LOW); // Switch the nickel wire off
    }

    sampling_delay.Start();
  }

  heating_delay.Update();
}