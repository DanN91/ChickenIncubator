#pragma once

#include "Runnable.h"

struct Delay {
  Delay(unsigned long duration, Runnable *callback);
  void Start();
  void Update();

private:
  unsigned long start_time_;
  unsigned long duration_;
  bool active_;
  Runnable *callback_;
};