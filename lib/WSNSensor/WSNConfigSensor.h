#ifndef wsn_config_sensor_h
#define wsn_config_sensor_h
#ifndef SENSOR_ID
#define SENSOR_ID 1
#endif
#include <Arduino.h>
#include <functional>

#define SENSOR_TYPE_TMP_102 1
#define SENSOR_TYPE_DHT_11 4
#define SENSOR_TYPE_GY_30 2

#ifndef I2C_ADDRESS_TMP_102
#define I2C_ADDRESS_TMP_102 0x48
#endif
#ifndef I2C_ADDRESS_GY_30
#define I2C_ADDRESS_GY_30 0x23
#endif

#ifndef SENSOR_TYPE
#define SENSOR_TYPE SENSOR_TYPE_DHT_11
#endif
#ifndef SENSOR_ENABLED
#define SENSOR_ENABLED true
#endif

typedef std::function<void(void)> sendSensorDataCallback_t;

struct WSNConfigSensor{
  int ID = SENSOR_ID;
  String label = "Luminosity";
  String unit = "lux";//"°C";
  bool sendDataEnabled = false;
  int sendDataIntervalMs = 30000;
  int tickerHandlerID = -1;
  sendSensorDataCallback_t callback = NULL;
  bool fireOnSetup = false;
  int type = SENSOR_TYPE;
  int i2cAddress = I2C_ADDRESS_TMP_102;
  bool enabled = SENSOR_ENABLED;
  float lastValue = 0;
  float threshold = 0.1;
};
#endif
