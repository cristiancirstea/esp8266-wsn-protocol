#ifndef WSN_SETUP_H_
#define WSN_SETUP_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "IPAddress.h"
#include <WiFiUdp.h>
#include <Wire.h>

#include "WSNConfig.h"
#include "TickerScheduler.h"
#include "Heartbeat.h"
#include "WSNHTTPServer.h"
#include "WSNOTA.h"

extern WSNConfig config;
extern ESP8266WebServer HTTPServer;
extern TickerScheduler tickerScheduler;

//TODO implement it in another file and include that here
extern void sendSensorData(); //this is implemented in common.h file

//TODO namespace or static method for WSN class

namespace WSN{
  void setup();
}

void setupWiFiSTA();
void setupWiFiAP();
void setupWiFi();
void setupUDPServer();
void setupTickerScheduler();
void setupHeartbeat();
void setupSensors();

#endif //WSN_SETUP_H_
