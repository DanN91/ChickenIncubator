#pragma once

#include "Runnable.h"

/**
 * Sampling callback class.
 * This class implements a callback function that is called after a sampling
 * delay.
 */
class SamplingCallback : public Runnable {
public:
  /**
   * Callback function.
   * This function is called after a sampling delay.
   */
  void Run() override;
};