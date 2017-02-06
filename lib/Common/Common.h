#ifndef WSN_COMMON_H
#define WSN_COMMON_H
#include <Arduino.h>
#include <Wire.h>
#include "WSNConfig.h"
#include "WSNClient.h"
#include "WSNPacket.h"
#include "WSNUDPServer.h"
#include "DHTSensor.h"
#include "GY30Sensor.h"

#ifndef NODE_LED_SIGNAL_DELAY
#define NODE_LED_SIGNAL_DELAY 80 /* x2 -> before on and before off*/
#endif

extern WSNConfig config;

namespace WSN {

  void setInternalLedOn();
  void setInternalLedOff();
  void logStage(String stage);
  void logMessage(String message);
  void logMessage(IPAddress ip);
  WSNSensorValue getSensorData();
  String ipToString(IPAddress ip);
  float distanceFromRSSI(double rssi);
  float getTMP102Value(int tmp102Address);
  void printBits(size_t const size, void const * const ptr);
  bool isIPFromSubnet(IPAddress theIP);
  void ledSignal(uint8_t signalCount);
  void ledSignal(uint8_t signalCount, unsigned long delayMilis);

  bool connectToWiFi(WiFiCredentials wifiCredentials);
  void scanWiFiAndConnect(bool forceDisconnect /* false */);
  int scanWiFiNetworks(WiFiCredentials* &networksFound);

  void sendHTTPToIFTTT();

  WSNUDPServer* UDPServer();
}

#endif //WSN_COMMON_H
