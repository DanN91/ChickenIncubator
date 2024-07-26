#include "HeatingCallback.h"

HeatingCallback::HeatingCallback(int pin) : pin_(pin), is_heating_(false) {}

void HeatingCallback::Run() { is_heating_ = false; }

void HeatingCallback::StartHeating(float output) {
  is_heating_ = true;
  output_ = output;
  digitalWrite(pin_, HIGH); // Switch the nickel wire on
}