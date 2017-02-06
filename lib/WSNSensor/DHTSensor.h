#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "DHT.h"

#define DHTPIN D4
#define DHTTYPE DHT11

#include "WSNSensor.h"

class DHTSensor : WSNSensor{
private:
  static DHT _dht;
public:
  static void setup();
  static WSNSensorValue value();
  static WSNSensorValue temperature();
  static WSNSensorValue humidity();
  static WSNSensorValue heatIndex();
};

#endif //DHT_SENSOR_H
