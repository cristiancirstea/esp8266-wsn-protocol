#ifndef GY_30_SENSOR_H
#define GY_30_SENSOR_H

#include "WSNSensor.h"
#include <Wire.h>

#define I2C_ADDRESS_GY_30 0x23

class GY30Sensor : public WSNSensor{
public:
  static void setup();
  static WSNSensorValue value();
};

#endif //GY_30_SENSOR_H
