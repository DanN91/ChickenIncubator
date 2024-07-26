#pragma once

#include "Runnable.h"

class HeatingCallback : public Runnable {
public:
  HeatingCallback(int pin);
  void Run() override;
  void StartHeating(float output);

private:
  bool is_heating_;
  int pin_;
  float output_;
};