#ifndef WSN_OTA_H
#define WSN_OTA_H

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "WSNConfig.h"
#include "Common.h"

namespace WSN{
  void setupOTA();
  void handleOTA();
}

#endif //WSN_OTA_H
