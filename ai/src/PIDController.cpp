#include "PidController.h"

/**
 * Constructor.
 * Initializes the PID controller with the given sensor, setpoint, and time
 * step.
 */
PidController::PidController(const Sensor &sensor, float setpoint, float dt)
    : sensor_(sensor), setpoint_(setpoint), kp_(1.0), ki_(0.1), kd_(0.01),
      previous_error_(0.0), integral_(0.0), dt_(dt) {}

/**
 * Set the new setpoint value.
 * Updates the setpoint value and resets the integral and previous error values.
 * @param setpoint The new setpoint value.
 */
void PidController::SetSetpoint(float setpoint) {
  setpoint_ = setpoint;
  integral_ = 0.0;
  previous_error_ = 0.0;
}

/**
 * Calculate the PID output value.
 * Calculates the PID output value based on the current error, integral, and
 * derivative values.
 * @return The calculated PID output value.
 */
float PidController::Calculate() {
  // Calculate the error value
  float error = setpoint_ - sensor_.GetValue();

  // Update the integral value
  integral_ += error * dt_;

  // Calculate the derivative value
  float derivative = (error - previous_error_) / dt_;

  // Update the previous error value
  previous_error_ = error;

  // Calculate the PID output value
  return kp_ * error + ki_ * integral_ + kd_ * derivative;
}

float PidController::CalculateHumidity() {
  // Calculate the error between the setpoint and the current humidity reading
  float error = setpoint_ - sensor_.GetHumidity();

  // Update the integral term by adding the error multiplied by the time step
  integral_ += error * dt_;

  // Calculate the derivative term by dividing the difference between the
  // current and previous errors by the time step
  float derivative = (error - previous_error_) / dt_;

  // Update the previous error value
  previous_error_ = error;

  // Calculate the PID output by combining the proportional, integral, and
  // derivative terms
  return kp_ * error + ki_ * integral_ + kd_ * derivative;
}