#pragma once

#include "Sensor.h"

/**
 * DHT sensor class.
 * This class represents a DHT sensor that provides temperature readings.
 */
class DhtSensor : public Sensor {
public:
  /**
   * Get the current temperature value.
   * @return The current temperature value.
   */
  float GetValue() const override;
};