#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
public:
  virtual float GetValue() const = 0;
};

#endif // SENSOR_H