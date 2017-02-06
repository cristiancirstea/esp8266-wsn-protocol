#ifndef wsn_http_server_h
#define wsn_http_server_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "IPAddress.h"

#include "WSNClient.h"
#include "WSNConfig.h"
#include "WSNSetup.h"
#include "WSNHeadNode.h"
#include "Common.h"

extern WSNConfig config;

extern ESP8266WebServer HTTPServer;

void setResponseHeaders();
void handle_root();
void handleNotFound();
void handle_led_on();
void handle_led_off();
void handle_sensor();
void handle_sensor_enable();
void handle_sensor_disable();
String http_arg_as_string(String argName /* defaultValue = "" */);
String http_arg_as_string(String argName, String defaultValue);
int http_arg_as_int(String argName /* defaultValue = 0 */);
int http_arg_as_int(String argName, int defaultValue);
bool http_wants_json();
bool http_has_arg(String argName);
void handle_wifi_config();
void handle_wifi_scan();
void handle_wifi_change_sta();
void handle_wifi_change_ap();
void handle_wifi_change_ap_sta();
void handle_status();
void handle_send_discover();
void handle_config();
void handle_restart();
void handle_battery_status();
void setupHTTPServer();

#endif
