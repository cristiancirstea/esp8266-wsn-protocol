#ifndef WSN_SENSOR_H
#define WSN_SENSOR_H

#include "Arduino.h"

struct WSNSensorValue{
  float value = 0;
  String label;
  String unit;
  WSNSensorValue(float v, String l, String u){
    value = v;
    label = l;
    unit = u;
  }
};

class WSNSensor{
  static void setup();
  static WSNSensorValue value();
};

#endif //WSN_SENSOR_H
