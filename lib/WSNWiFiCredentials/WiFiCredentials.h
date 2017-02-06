#ifndef wificredentials_h
#define wificredentials_h
#define WIFI_NO_SIGNAL_RSSI -110
#include "IPAddress.h"

#ifndef WSN_SSID_HEAD_NODE_PREFIX
#define WSN_SSID_HEAD_NODE_PREFIX "HN-ESP8266"
#endif
#ifndef WSN_SSID_NODE_PREFIX
#define WSN_SSID_NODE_PREFIX "N-ESP8266"
#endif

struct WiFiCredentials{
  char SSID[32];
  char password[64];
  bool useStaticIP;
  int32_t rssi = WIFI_NO_SIGNAL_RSSI;
  //TODO remove defaults
  IPAddress staticIP = IPAddress(192,168,44,109);
  IPAddress gateway = IPAddress(192,168,44,1);
  IPAddress mask = IPAddress(255,255,255,0);
  IPAddress broadcast = IPAddress(192,168,44,255);

  WiFiCredentials(){
      snprintf(SSID, sizeof(SSID), "%s", "Undefined");
      this->useStaticIP = false;
  }
  WiFiCredentials(const char* ssid, const char* passw)
  {
    snprintf(SSID, sizeof(SSID), "%s", ssid);
    snprintf(password, sizeof(password), "%s", passw);
    this->useStaticIP = false;
  }

  WiFiCredentials(const char* ssid, const char* passw, IPAddress ip, IPAddress gateway, IPAddress mask, IPAddress broadcast)
    : WiFiCredentials(ssid, passw)
  {
    this->useStaticIP = true;
    this->staticIP = ip;
    this->gateway = gateway;
    this->mask = mask;
    this->broadcast = broadcast;
  }

  WiFiCredentials(const char* ssid, const char* passw, IPAddress broadcast)
    : WiFiCredentials(ssid, passw)
  {
    this->useStaticIP = false;
    this->broadcast = broadcast;
  }

  void setSSID(const char* ssid)
  {
      snprintf(SSID, sizeof(SSID), "%s", ssid);
  }

  void setPassword(const char* passw)
  {
    snprintf(password, sizeof(password), "%s", passw);
  }

  bool isFromHeadNode()
  {
    const char* headNodeSignature = WSN_SSID_HEAD_NODE_PREFIX;
    char* output = NULL;
    output = strstr(this->SSID, headNodeSignature);
    if(output)
    {
        return true;
    }
    return false;
  }

  bool isFromNode()
  {
    const char* nodeSignature = WSN_SSID_NODE_PREFIX;
    char* output = NULL;
    output = strstr(this->SSID, nodeSignature);
    if(output)
    {
        return true;
    }
    return false;
  }

};
#endif
