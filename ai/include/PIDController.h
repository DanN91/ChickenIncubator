#pragma once

#include "Sensor.h"

/**
 * PID (Proportional-Integral-Derivative) controller class.
 * This class implements a PID controller algorithm to control a system.
 */
class PidController {
public:
  /**
   * Constructor.
   * @param sensor The sensor object to read the current value from.
   * @param setpoint The desired setpoint value.
   * @param dt The time step (in seconds) for the PID calculation.
   */
  PidController(const Sensor &sensor, float setpoint, float dt);

  /**
   * Set the new setpoint value.
   * @param setpoint The new setpoint value.
   */
  void SetSetpoint(float setpoint);

  /**
   * Calculate the PID output value for maintaining the temperature.
   * @return The calculated PID output value.
   */
  float CalculateTemperature();

  /**
   * Calculate the PID output value for maintaining the humidity.
   * @return The calculated PID output value.
   */
  float CalculateHumidity();

private:
  const Sensor &sensor_; ///< The sensor object.
  float setpoint_;       ///< The desired setpoint value.
  float kp_;             ///< The proportional gain.
  float ki_;             ///< The integral gain.
  float kd_;             ///< The derivative gain.
  float previous_error_; ///< The previous error value.
  float integral_;       ///< The integral value.
  const float dt_; ///< The time step (in seconds) for the PID calculation.
};