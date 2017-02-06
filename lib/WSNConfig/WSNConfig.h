#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include "WSNConfigNode.h"
#include "WSNConfigSensor.h"
#include "WSNConfigSink.h"
#include "WSNWiFiConfig.h"
#include "WSNHTTPConfig.h"
#include "WSNUDPConfig.h"
#include "WSNNetworkConfig.h"
#include "TickerSchedulerConfig.h"
#include "HeartbeatConfig.h"
#include "WSNNodeLink.h"

struct WSNConfig{
  bool DEBUG = true;

  const int internalLedPin = 2;
  bool HTTPServerStarted = false;
  bool UDPServerStarted = false;

  bool registeredToSink = false;

  bool enabledOTA = true;

  WSNConfigNode node;
  WSNWiFiConfig wifi;
  TickerSchedulerConfig ticker;
  WSNConfigSink sink;
  WSNUDPConfig udp;
  WSNHTTPConfig tcp;  //TODO rename to http
  HeartbeatConfig heartbeat;
  WSNConfigSensor sensor;
  WSNNetworkConfig network;
  WSNNodeLink parent;
};

extern WSNConfig config;

#endif //WIFI_CONFIG_H
