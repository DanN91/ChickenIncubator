#include "Delay.h"

Delay::Delay(unsigned long duration, Runnable *callback)
    : duration_(duration), callback_(callback), active_(false) {}

void Delay::Start() {
  start_time_ = millis();
  active_ = true;
}

void Delay::Update() {
  if (active_ && millis() - start_time_ >= duration_) {
    callback_->Run();
    active_ = false;
  }
}