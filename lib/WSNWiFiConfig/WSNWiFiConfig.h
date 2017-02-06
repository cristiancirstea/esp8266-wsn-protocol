#ifndef wsn_config_wifi_h
#define wsn_config_wifi_h

#include <ESP8266WiFi.h>
#include "WiFiCredentials.h"


#define WIFI_CREDENTIALS_MAX_COUNT 5
#define WIFI_CONNECTION_RETRIES_MAX 20
#define WIFI_CONNECTION_RETRIES_DELAY 500
#ifndef NODE_SUBNET_SUFFIX
#define NODE_SUBNET_SUFFIX 109
#endif
#ifndef NODE_WIFI_MODE
#define NODE_WIFI_MODE WIFI_AP_STA
#endif

struct WSNWiFiConfig{
  WiFiMode mode = NODE_WIFI_MODE; //WIFI_STA, WIFI_AP, WIFI_AP_STA
  int subnetSufix = NODE_SUBNET_SUFFIX;
  int credentialsCount = 1;
  //priority is from last to first
  WiFiCredentials credentials[WIFI_CREDENTIALS_MAX_COUNT] =
  {
    WiFiCredentials(
      "SSID",
      "PASSWORD",
      IPAddress(192,168,0,NODE_SUBNET_SUFFIX), //Static IP
      IPAddress(192,168,0,1),   //Gateway
      IPAddress(255,255,255,0),  //Mask
      IPAddress(192,168,0,255)  //Broadcast
    )
  };
  int _credentialsUsed = -1;

  IPAddress apIP;
  IPAddress apBroadcastIP = IPAddress(192,168,4,255);

  #ifdef NODE_AP_SSID
  char apSSID[32] = NODE_AP_SSID;
  #else
  char apSSID[32] = WSN_SSID_NODE_PREFIX;
  #endif
  char apPassword[64] = "test12345678";

  uint8_t modeInt8(){
    switch(mode)
    {
      case WIFI_OFF:
        return 0;
      case WIFI_STA:
        return 1;
      case WIFI_AP:
        return 2;
      case WIFI_AP_STA:
        return 3;
    }
    return 0;
  }

  String modeString()
  {
    switch(mode)
    {
      case WIFI_OFF:
        return "WIFI_OFF";
      case WIFI_STA:
        return "WIFI_STA";
      case WIFI_AP:
        return "WIFI_AP";
      case WIFI_AP_STA:
        return "WIFI_AP_STA";
    }
    return "";
  }

  WiFiCredentials getCredentialsUsed()
  {
    if(!this->isConnected())
    {
      //AP credentials
      return WiFiCredentials(
        apSSID,
        apPassword,
        IPAddress(192,168,4,1), //Static IP
        IPAddress(192,168,4,1),   //Gateway
        IPAddress(255,255,255,0),  //Mask
        IPAddress(192,168,4,255)  //Broadcast
      ); //TODO
    }
    return this->credentials[_credentialsUsed];
  }

  void setCredentialsUsed(int credentialIndex)
  {
    this->_credentialsUsed = credentialIndex;
  }

  bool isConnected()
  {
    return (WiFi.status() == WL_CONNECTED && this->_credentialsUsed >= 0);
  }

  void setDisconnected()
  {
    this->_credentialsUsed = -1;
  }

  IPAddress getLocalIP()
  {
    return WiFi.localIP();
  }

  IPAddress getAPIP()
  {
    return WiFi.softAPIP();
  }

  bool isAPMode()
  {
    return (mode == WIFI_AP_STA || mode == WIFI_AP);
  }
};
#endif
