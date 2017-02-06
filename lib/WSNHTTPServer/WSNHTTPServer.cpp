#include "WSNHTTPServer.h"
#include <Arduino.h>
#include "WSNSensor.h"

ESP8266WebServer HTTPServer(config.tcp.port);

void setupHTTPServer()
{
  WSN::logStage("HTTP SERVER");

  HTTPServer.on("/", handle_root);
  HTTPServer.on("/led/on", handle_led_on);
  HTTPServer.on("/led/off", handle_led_off);
  HTTPServer.on("/sensor", handle_sensor);
  HTTPServer.on("/sensor/enable", handle_sensor_enable);
  HTTPServer.on("/sensor/disable", handle_sensor_disable);
  HTTPServer.on("/wifi/sta", handle_wifi_change_sta);
  HTTPServer.on("/wifi/ap", handle_wifi_change_ap);
  HTTPServer.on("/wifi/apsta", handle_wifi_change_ap_sta);
  HTTPServer.on("/wifi/config", handle_wifi_config);
  HTTPServer.on("/wifi/scan", handle_wifi_scan);
  HTTPServer.on("/status", handle_status);
  HTTPServer.on("/discover", handle_send_discover);
  HTTPServer.on("/config", handle_config);
  HTTPServer.on("/restart", handle_restart);
  HTTPServer.on("/battery/status", handle_battery_status);

  HTTPServer.onNotFound(handleNotFound);

  HTTPServer.begin();
  config.HTTPServerStarted = true;

  Serial.println(String("HTTP server started on port: ") + config.tcp.port);
}

void setResponseHeaders()
{
  HTTPServer.sendHeader("Content-Type", "application/json; charset=utf-8");
  HTTPServer.sendHeader("Access-Control-Allow-Credentials", "true");
  HTTPServer.sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, HEAD");
  HTTPServer.sendHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Methods,  Access-Control-Allow-Origin");
  HTTPServer.sendHeader("WSN-Node-ID", String(config.node.ID));
}

void handle_battery_status()
{
    String responseMessage = "{";

    int value =  analogRead(A0);
    float voltage = value * 4.2 / 1024;

    responseMessage += String("\"battery\" : " ) + String(voltage);

    responseMessage += String(", \"value\" : ") + String(value);
    responseMessage += String(", \"percent\" : \"") + String(voltage / 4.2 * 100) + "%\"";
    responseMessage += "}";

    HTTPServer.send(200, "text/html", responseMessage);
    // delay(100);
}

void handle_root()
{
  // setResponseHeaders();
  String responseMessage = "<!DOCTYPE html>\n<html>";
  responseMessage += "<head>";
  responseMessage += String("<title>WSN ") + config.node.label + "</title>";
  responseMessage += "</head>";
  responseMessage += "<body>";

  responseMessage += String("<h2>Hello from ESP8266-") + config.node.getTypeToString() + " "  + config.node.label +".</h2><br>";
  responseMessage += "Available links are:<br>";
  responseMessage += "<ul>";
  responseMessage += "<li><a href=\"\">Display this message. -> <b>/</b></a></li><br>";
  responseMessage += "<li><a href=\"/led/on\">Turn internal LED ON. -> <b>/led/on</b></a></li><br>";
  responseMessage += "<li><a href=\"/led/off\">Turn internal LED OFF. -> <b>/led/off </b></a></li><br>";
  responseMessage += "<li><a href=\"/sensor\">Get data from sensor[TODO] -> <b>/sensor</b></a>.</li><br>";
  responseMessage += "<li><a href=\"/sensor/enable\">Enable sending sensor data to sink. -> <b>/sensor/enable</b></a></li><br>";
  responseMessage += "<li><a href=\"/sensor/disable\">Disable sending sensor data to sink. -> <b>/sensor/disable</b></a></li><br>";
  responseMessage += "<li><a href=\"/wifi/sta\">Change WiFi mode to STA (Client only) -> <b>/wifi/sta</b></a>.</li><br>";
  responseMessage += "<li><a href=\"/wifi/ap\">Change WiFi mode to AP (Access Point only) -> <b>/wifi/ap</b></a>.</li><br>";
  responseMessage += "<li><a href=\"/wifi/apsta\">Change WiFi mode to APSTA (Client + Access Point) -> <b>/wifi/apsta</b></a>.</li><br>";
  responseMessage += "<li><a href=\"/wifi/config\">Config WiFi parameters. -> <b>/wifi/config</b></a></li><br>";
  responseMessage += "<li><a href=\"/wifi/scan\">Scan WiFi networks and display RSSI and Aproximate Distance. -> <b>/wifi/scan</b></a></li><br>";
  responseMessage += "<li><a href=\"/status\">Show node status. -> <b>/status</b></a></li><br>";
  responseMessage += "<li><a href=\"/discover\">Send Discover message. -> <b>/discover</b></a></li><br>";
  responseMessage += "<li><a href=\"/config\">All Config. -> <b>/config</b></a></li><br>";
  responseMessage += "<li><a href=\"/restart\">Restart ESP. -> <b>/restart</b></a></li><br>";
  responseMessage += "</ul>";
  responseMessage += "<p>Send parameter \"json\" to encode response message to JSON. </p>";

  responseMessage += "<body></html>";

  HTTPServer.send(200, "text/html", responseMessage);
  delay(100);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += HTTPServer.uri();
  message += "\nMethod: ";
  message += (HTTPServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += HTTPServer.args();
  message += "\n";
  for (uint8_t i=0; i<HTTPServer.args(); i++){
    message += " " + HTTPServer.argName(i) + ": " + HTTPServer.arg(i) + "\n";
  }
  Serial.println("NOT FOUND\n " + message);
  HTTPServer.send(404, "text/plain", message);
}

void handle_led_on()
{
  setResponseHeaders();
  WSN::setInternalLedOn();
  HTTPServer.send(200, "application/json", "{\"state\" : 1}");
  delay(100);
}

void handle_led_off()
{
  setResponseHeaders();
  WSN::setInternalLedOff();

  HTTPServer.send(200, "application/json", "{\"state\" : 0}");
  delay(100);
}

void handle_sensor()
{
  setResponseHeaders();
  WSNSensorValue sensorValue = WSN::getSensorData();
  HTTPServer.send(200, "application/json",
    String("{\"value\" :") +  sensorValue.value + "," +
    " \"unit\": \"" + sensorValue.unit + "\", " +
    "\"label\": \"" + sensorValue.label +"\" ,"+
    "\"enabled\" : " + (config.sensor.sendDataEnabled ? "true" : "false" ) + "}");
  delay(100);
}

void handle_sensor_enable()
{
  setResponseHeaders();
  config.sensor.sendDataEnabled = true;
  HTTPServer.send(200, "application/json", "{\"message\" : \"Sensor enabled for creating new data.\"}");
  delay(100);
  Serial.println("Sensor send data ENABLED.");
  setupTickerScheduler();
}

void handle_sensor_disable()
{
  setResponseHeaders();
  config.sensor.sendDataEnabled = false;
  HTTPServer.send(200, "application/json", "{\"message\" : \"Sensor disabled for creating new data.\"}");
  delay(100);
  Serial.println("Sensor send data DISABLED.");
}

void handle_wifi_config()
{
  //TODO still working on this
  String responseMessage = "<!DOCTYPE html>\n<html>";

  responseMessage += "<head>";
  responseMessage += "<title>WSN Node WiFi Config</title>";
  responseMessage += "<style type=\"text/css\">\
      label, button{\
          width: 65px;\
          display: inline-block;\
          margin: 5px;\
        }\
      .ip{\
        width: 36px;\
        text-align: center;\
      }\
      button{\
        width: 248px;\
        height: 30px;\
        margin-top: 15px;\
      }\
      form{\
        width: 255px;\
        margin-left: auto;\
        margin-right: auto;\
      }\
      h3{\
        width: 255px;\
        margin-left: auto;\
        margin-right: auto;\
      }\
    </style>";
  responseMessage += "</head>";
  responseMessage += "<body>";

  if(HTTPServer.args() <= 0)
  {
    responseMessage += "<h3>Node WiFi Config</h3>";
    responseMessage += "<br>";
    responseMessage += "<form action=\"/wifi/config\" method=\"POST\">";

    responseMessage += "<label for=\"input-ssid\">SSID</label> ";
    responseMessage += "<input name=\"ssid\" type=\"text\" id=\"input-ssid\">";
    responseMessage += "<br>";

    responseMessage += "<label for=\"input-password\">Password</label> ";
    responseMessage += "<input name=\"password\" type=\"password\" id=\"input-password\">";
    responseMessage += "<br>";

    responseMessage += "<input type=\"checkbox\" name=\"static\"> Static IP";
    responseMessage += "<br>";

    responseMessage += "<label for=\"input-ips1\">Static IP</label> ";
    responseMessage += "<input class=\"ip\" name=\"ips1\" type=\"text\" id=\"input-ips1\" value=\"192\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"ips2\" type=\"text\" id=\"input-ips2\" value=\"168\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"ips3\" type=\"text\" id=\"input-ips3\" value=\"44\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"ips4\" type=\"text\" id=\"input-ips4\" value=\"108\">";
    responseMessage += "<br>";

    responseMessage += "<label for=\"input-gatewaw1\">Gateway</label> ";
    responseMessage += "<input class=\"ip\" name=\"gatew1\" type=\"text\" id=\"input-ips1\" value=\"192\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"gatew2\" type=\"text\" id=\"input-gatew2\" value=\"168\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"gatew3\" type=\"text\" id=\"input-gatew3\" value=\"44\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"gatew4\" type=\"text\" id=\"input-gatew4\" value=\"1\">";
    responseMessage += "<br>";

    responseMessage += "<label for=\"input-mask1\">Mask</label> ";
    responseMessage += "<input class=\"ip\" name=\"mask1\" type=\"text\" id=\"input-mask1\" value=\"255\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"mask2\" type=\"text\" id=\"input-mask2\" value=\"255\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"mask3\" type=\"text\" id=\"input-mask\" value=\"255\">";
    responseMessage += ":";
    responseMessage += "<input class=\"ip\" name=\"mask4\" type=\"text\" id=\"input-mask4\" value=\"255\">";
    responseMessage += "<br>";

    responseMessage += "<button type=\"submit\">Submit</button>";
    responseMessage += "</form>";
  }
  else
  {
    WiFiCredentials newCredentials;

    http_arg_as_string("ssid", "").toCharArray(newCredentials.SSID, 20);
    http_arg_as_string("password", "").toCharArray(newCredentials.password, 20);
    newCredentials.staticIP = IPAddress(http_arg_as_int("ips1", 192), http_arg_as_int("ips2", 168), http_arg_as_int("ips3", 4), http_arg_as_int("ips4", 2));
    newCredentials.gateway = IPAddress(http_arg_as_int("gatew1", 192), http_arg_as_int("gatew2", 168), http_arg_as_int("gatew3", 4), http_arg_as_int("gatew4", 1));
    newCredentials.mask = IPAddress(http_arg_as_int("mask1", 255), http_arg_as_int("mask2", 255), http_arg_as_int("mask3", 255), http_arg_as_int("mask4", 0));
    newCredentials.broadcast = IPAddress(http_arg_as_int("ips1", 192), http_arg_as_int("ips2", 168), http_arg_as_int("ips3", 4), 255);
    newCredentials.useStaticIP = http_has_arg("static");

    responseMessage += String("<p>SSID changed to ") + newCredentials.SSID + "</p>";
    responseMessage += String("<p>Password changed to ") + newCredentials.password + "</p>";
    responseMessage += String("<p>IP changed to ") + WSN::ipToString(newCredentials.staticIP) + "</p>";
    responseMessage += String("<p>Gateway changed to ") + WSN::ipToString(newCredentials.gateway) + "</p>";
    responseMessage += String("<p>Mask changed to ") + WSN::ipToString(newCredentials.mask) + "</p>";

    config.wifi.credentials[config.wifi.credentialsCount] = newCredentials;

    //overrides the last one if there is no space in credentials array
    if(config.wifi.credentialsCount + 1 < WIFI_CREDENTIALS_MAX_COUNT)
    {
      config.wifi.credentialsCount++;
    }
    if(config.wifi.isConnected())
    {
      WiFi.disconnect(true);
      config.wifi.setDisconnected();
    }

    responseMessage += "</body>";
    responseMessage += "</html>";

    HTTPServer.send(200, "text/html", responseMessage);

    setupWiFi();
    return;
  }

  responseMessage += "</body>";
  responseMessage += "</html>";

  HTTPServer.send(200, "text/html", responseMessage);
}

void handle_config()
{
    String responseMessage = "<!DOCTYPE html>\n<html>";
    responseMessage += "<head>";
    responseMessage += "<title>WSN Node Config</title>";
    responseMessage += "<style type=\"text/css\">\
      label, button{\
          width: 120px;\
          display: inline-block;\
          margin: 5px;\
        }\
      input, select{\
        width: 220px;\
      }\
      input[type=\"checkbox\"]{\
        width: initial;\
      }\
      .ip{\
        width: 40px;\
        text-align: center;\
      }\
      button{\
        width: 400px;\
        height: 30px;\
        margin-top: 15px;\
      }\
      form{\
        width: 400px;\
        margin-left: auto;\
        margin-right: auto;\
      }\
      h3{\
        width: 400px;\
        margin-left: auto;\
        margin-right: auto;\
      }\
      </style>";
    responseMessage += "</head>";
    responseMessage += "<body>";

    if(HTTPServer.args() <= 0)
    {
      responseMessage += "<h3>Node Config</h3>";
      responseMessage += "</br>";

      responseMessage += "<form action=\"/config\" method=\"POST\">";
      responseMessage += String("<label for=\"input-node-label\">Node Label </label>");
      responseMessage += String("<input id=\"input-node-label\" type=\"text\" name=\"node_label\" value=\"")+ config.node.label + "\"/>";
      responseMessage += "<br>";

      responseMessage += String("<label for=\"input-node-id\">Node ID </label>");
      responseMessage += String("<input min=\"1\" id=\"input-node-id\" type=\"number\" name=\"node_id\" value=\"")+ config.node.ID + "\"/>";
      responseMessage += "<br>";

      responseMessage += String("<label for=\"input-node-type\">Node Type </label>");
      responseMessage += String("<select id=\"input-node-type\" name=\"node_type\">") +
                                  "<option value=\"0\" " + (config.node.getType() == 0 ? "selected" : "") + ">Node</option>" +
                                  "<option value=\"1\" " + (config.node.getType() == 1 ? "selected" : "") + ">HeadNode</option>" +
                                  "<option value=\"3\" " + (config.node.getType() == 3 ? "selected" : "") + ">Sink</option>" +
                                "</select>";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-network-type\">Network Type</label> ";
      responseMessage += String("<select id=\"input-network-type\" name=\"network_type\">") +
                                  "<option value=\"1\" " + (config.network.type == 1 ? "selected" : "") + ">Star</option>" +
                                  "<option value=\"2\" " + (config.network.type == 2 ? "selected" : "") + ">Tree</option>" +
                                  "<option value=\"3\" " + (config.network.type == 3 ? "selected" : "") + ">Mesh</option>" +
                                "</select>";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-wifi-mode\">WiFi Mode</label> ";
      responseMessage += String("<select id=\"input-wifi-mode\" name=\"wifi_mode\">") +
                                  "<option value=\"1\" " + (config.wifi.modeInt8() == 1 ? "selected" : "") + ">WIFI_STA</option>" +
                                  "<option value=\"2\" " + (config.wifi.modeInt8() == 2 ? "selected" : "") + ">WIFI_AP</option>" +
                                  "<option value=\"3\" " + (config.wifi.modeInt8() == 3 ? "selected" : "") + ">WIFI_AP_STA</option>" +
                                "</select>";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-ap-ssid\">AP SSID</label> ";
      responseMessage += String("<input name=\"ap_ssid\" type=\"text\" id=\"input-ap-ssid\" value=\"") + config.wifi.apSSID +"\">";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-ap-password\">AP Password</label> ";
      responseMessage += String("<input name=\"ap_password\" type=\"text\" id=\"input-ap-password\" value=\"") + config.wifi.apPassword + "\">";
      responseMessage += "<br>";


      responseMessage += "<label for=\"input-ssid\">SSID</label> ";
      responseMessage += String("<input name=\"ssid\" type=\"text\" id=\"input-ssid\" value=\"") + config.wifi.getCredentialsUsed().SSID +"\">";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-password\">Password</label> ";
      responseMessage += String("<input name=\"password\" type=\"password\" id=\"input-password\" value=\"") + config.wifi.getCredentialsUsed().password + "\">";
      responseMessage += "<br>";

      responseMessage += String("<input type=\"checkbox\" name=\"static\"") + (config.wifi.getCredentialsUsed().useStaticIP ? "checked": "") + "> Static IP";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-ips1\">Static IP</label> ";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"ips1\" type=\"number\" id=\"input-ips1\" value=\"") + config.wifi.getLocalIP()[0] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"ips2\" type=\"number\" id=\"input-ips2\" value=\"") + config.wifi.getLocalIP()[1] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"ips3\" type=\"number\" id=\"input-ips3\" value=\"") + config.wifi.getLocalIP()[2] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"ips4\" type=\"number\" id=\"input-ips4\" value=\"") + config.wifi.getLocalIP()[3] + "\">";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-gatewaw1\">Gateway</label> ";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"gatew1\" type=\"number\" id=\"input-ips1\" value=\"") + config.wifi.getCredentialsUsed().gateway[0] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"gatew2\" type=\"number\" id=\"input-gatew2\" value=\"") + config.wifi.getCredentialsUsed().gateway[1] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"gatew3\" type=\"number\" id=\"input-gatew3\" value=\"") + config.wifi.getCredentialsUsed().gateway[2] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"gatew4\" type=\"number\" id=\"input-gatew4\" value=\"") + config.wifi.getCredentialsUsed().gateway[3] + "\">";
      responseMessage += "<br>";

      responseMessage += "<label for=\"input-mask1\">Mask</label> ";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"mask1\" type=\"number\" id=\"input-mask1\" value=\"") + config.wifi.getCredentialsUsed().mask[0] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"mask2\" type=\"number\" id=\"input-mask2\" value=\"") + config.wifi.getCredentialsUsed().mask[1] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"mask3\" type=\"number\" id=\"input-mask\" value=\"") + config.wifi.getCredentialsUsed().mask[2] + "\">";
      responseMessage += ":";
      responseMessage += String("<input min=\"0\" class=\"ip\" name=\"mask4\" type=\"number\" id=\"input-mask4\" value=\"") + config.wifi.getCredentialsUsed().mask[3] + "\">";
      responseMessage += "<br>";


      responseMessage += "<button type=\"submit\">Submit</button>";
      responseMessage += "</form>";
    }
    else
    {

      config.node.ID = http_arg_as_int("node_id", config.node.ID);
      http_arg_as_string("node_label", String(config.node.label)).toCharArray(config.node.label, 50); //TODO
      config.node.setType(http_arg_as_int("node_type", config.node.getType()));
      config.network.type = http_arg_as_int("network_type", config.network.type);
      uint8_t wifiMode = http_arg_as_int("wifi_mode", config.wifi.modeInt8());
      switch (wifiMode) {
        case 0:
          config.wifi.mode = WIFI_OFF;
          break;
        case 1:
          config.wifi.mode = WIFI_STA;
          break;
        case 2:
          config.wifi.mode = WIFI_AP;
          break;
        case 3:
          config.wifi.mode = WIFI_AP_STA;
          break;
      }

      responseMessage += String("<p>Node ID: <b>") + config.node.ID + "</b></p>";
      responseMessage += String("<p>Node Label: <b>") + config.node.label + "</b></p>";
      responseMessage += String("<p>Node Type: <b>") + config.node.getTypeToString() + "</b></p>";
      responseMessage += String("<p>Network Type: <b>") + config.network.typeToString() + "</b></p>";
      responseMessage += String("<p>WiFi Mode: <b>") + config.wifi.modeString() + "</b></p>";



      WiFiCredentials newCredentials;

      http_arg_as_string("ap_ssid", "").toCharArray(config.wifi.apSSID, 20);
      http_arg_as_string("ap_password", "").toCharArray(config.wifi.apPassword, 20);
      http_arg_as_string("ssid", "").toCharArray(newCredentials.SSID, 20);
      http_arg_as_string("password", "").toCharArray(newCredentials.password, 20);
      newCredentials.staticIP = IPAddress(http_arg_as_int("ips1", 192), http_arg_as_int("ips2", 168), http_arg_as_int("ips3", 4), http_arg_as_int("ips4", 2));
      newCredentials.gateway = IPAddress(http_arg_as_int("gatew1", 192), http_arg_as_int("gatew2", 168), http_arg_as_int("gatew3", 4), http_arg_as_int("gatew4", 1));
      newCredentials.mask = IPAddress(http_arg_as_int("mask1", 255), http_arg_as_int("mask2", 255), http_arg_as_int("mask3", 255), http_arg_as_int("mask4", 0));
      newCredentials.broadcast = IPAddress(http_arg_as_int("ips1", 192), http_arg_as_int("ips2", 168), http_arg_as_int("ips3", 4), 255);
      newCredentials.useStaticIP = http_has_arg("static");

      responseMessage += String("<p>SSID: <b>") + newCredentials.SSID + "</b></p>";
      responseMessage += String("<p>Password: <b>") + newCredentials.password + "</b></p>";
      responseMessage += String("<p>IP: <b>") + WSN::ipToString(newCredentials.staticIP) + "</b></p>";
      responseMessage += String("<p>Gateway: <b>") + WSN::ipToString(newCredentials.gateway) + "</b></p>";
      responseMessage += String("<p>Mask: <b>") + WSN::ipToString(newCredentials.mask) + "</b></p>";

      config.wifi.credentials[config.wifi.credentialsCount] = newCredentials;

      //overrides the last one if there is no space in credentials array
      if(config.wifi.credentialsCount + 1 < WIFI_CREDENTIALS_MAX_COUNT)
      {
        config.wifi.credentialsCount++;
      }
      if(config.wifi.isConnected())
      {
        WiFi.disconnect(true);
        config.wifi.setDisconnected();
      }

      responseMessage += "</body>";
      responseMessage += "</html>";

      HTTPServer.send(200, "text/html", responseMessage);

      WSN::setup();
      return;
    }

    responseMessage += "</body>";
    responseMessage += "</html>";

    HTTPServer.send(200, "text/html", responseMessage);
  }

void handle_wifi_scan()
{
  setResponseHeaders();
  String responseMessage;

  WiFiCredentials* networksFound;

  int availableNetworks = WSN::scanWiFiNetworks(networksFound);

  bool encodeJSON = http_wants_json();

  responseMessage += encodeJSON ? "[" : "WiFi Status\n\n";

  for(int networkIndex = 0; networkIndex < availableNetworks; networkIndex++)
  {
    double d = WSN::distanceFromRSSI(networksFound[networkIndex].rssi);

    if(encodeJSON)
    {
      responseMessage += "{";
      responseMessage += String("\"ssid\" : \"") + networksFound[networkIndex].SSID + "\" , ";
      responseMessage += String("\"rssi\" : \"") + networksFound[networkIndex].rssi + "dBm\",";
      responseMessage += String(" \"distance\" : \"") + d +"m\"";
      responseMessage += "}";
    }
    else
    {
      responseMessage += String(networksFound[networkIndex].SSID) + ": " + networksFound[networkIndex].rssi + "dbm ~" + d + "m\n";
    }

    if(networkIndex < availableNetworks - 1)
    {
        responseMessage += encodeJSON ? "," : "";
    }
  }

  responseMessage += encodeJSON ? "]" : "";

  HTTPServer.send(200, encodeJSON ? "application/json" : "text/plain", responseMessage);
}



void handle_wifi_change_sta()
{
  config.wifi.mode = WIFI_STA;
  setupWiFi();
}

void handle_wifi_change_ap()
{
  config.wifi.mode = WIFI_AP;
  setupWiFi();
}

void handle_wifi_change_ap_sta()
{
  config.wifi.mode = WIFI_AP_STA;
  setupWiFi();
}

void handle_status()
{
  String responseMessage = "General Status\n\n";
  responseMessage += String("Node ID: ") + config.node.ID + "\n";
  responseMessage += String("Node role: ") + config.node.getTypeToString() + "\n";
  responseMessage += String("Node label: ") + String(config.node.label) + "\n\n";
  responseMessage += String("Network type: ") + config.network.typeToString() + "\n\n";
  responseMessage += String("Sensor ID: ") + config.sensor.ID + "\n";
  responseMessage += String("Sensor Label: ") + config.sensor.label + "\n\n";
  responseMessage += String("Heartbeat enabled: ") + ((config.heartbeat.enabled) ? "true" : "false") + "\n";
  responseMessage += String("Heartbeat port: ") + config.heartbeat.UDPPort + "\n";
  responseMessage += String("Heartbeat interval: ") + config.heartbeat.intervalMs + "ms\n";

  responseMessage += "\n-------------------------------------------------------\n";
  responseMessage += "WiFi Status\n\n";

  responseMessage += "WiFi Mode: " + config.wifi.modeString() + "\n\n";

  if(config.wifi.isConnected())
  {
    WiFiCredentials credentials = config.wifi.getCredentialsUsed();
    responseMessage += String("Connected to:  ") + credentials.SSID + "\n";

    if(config.wifi.mode == WIFI_STA || config.wifi.mode == WIFI_AP_STA)
    {
      responseMessage += String("IP: ") + WSN::ipToString(config.wifi.getLocalIP()) + "\n\n";
      if(credentials.staticIP)
      {
        responseMessage += "Static\n\n";
        responseMessage += String("Gateway: ") + WSN::ipToString(credentials.gateway) + "\n";
        responseMessage += String("Mask: ") + WSN::ipToString(credentials.mask) + "\n";
      }
      else
      {
        responseMessage += "DHCP enabled\n";
      }
      responseMessage += String("Broadcast: ") + WSN::ipToString(credentials.broadcast) + "\n\n";
    }
  }
  else
  {
    responseMessage += String("Not connected to any WiFi!") + "\n";
  }

  if(config.wifi.mode == WIFI_AP || config.wifi.mode == WIFI_AP_STA)
  {
    responseMessage += String("AP SSID: ") + config.wifi.apSSID + "\n";
    responseMessage += String("AP password: ") + config.wifi.apPassword + "\n";
    responseMessage += String("AP IP: ") + WSN::ipToString(config.wifi.getAPIP()) + "\n";
  }


  responseMessage += "\n-------------------------------------------------------\n";

  responseMessage += "Parent Node Link\n";

  if(config.parent.isSet())
  {
    responseMessage += String("\n") + config.parent.toString() + "\n";
  }
  else
  {
    responseMessage += "-\n";
  }

  WSNNodeLink* nodeLinkIterator;

  responseMessage += "\nAll Child Node Links\n\n";
  if(WSNChildNodeLinks::isEmpty())
  {
    responseMessage += "-\n";
  }
  else
  {
    nodeLinkIterator = WSNChildNodeLinks::getFirst();
    while(nodeLinkIterator != NULL)
    {
      responseMessage += nodeLinkIterator->toString() + "\n";
      nodeLinkIterator = nodeLinkIterator->next;
    }
  }

  responseMessage += "\nAll Node Links\n\n";
  if(WSNNodeLinks::isEmpty())
  {
    responseMessage += "-\n";
  }
  else
  {
    nodeLinkIterator = WSNNodeLinks::getFirst();
    while(nodeLinkIterator != NULL)
    {
      responseMessage += nodeLinkIterator->toString() + "\n";
      nodeLinkIterator = nodeLinkIterator->next;
    }
  }

  HTTPServer.send(200, "text/plain", responseMessage);
}

void handle_send_discover()
{
  //TODO use a factory
  if(config.node.getType() == WSN_NODE_TYPE_NODE)
    WSNNode::discover();
  else
    WSNHeadNode::discover();

  HTTPServer.send(200, "text/plain", "Discover sent.");
}

void handle_restart()
{
  Serial.println("Rebooting...");
  ESP.restart();
  HTTPServer.send(200, "text/html", "Restarted!");
}

bool http_has_arg(String argName)
{
  for(uint8_t i = 0; i < HTTPServer.args(); i++)
  {
    if(HTTPServer.argName(i) == argName)
    {
      return true;
    }
  }
  return false;
}

bool http_wants_json()
{
  return http_has_arg("json");
}

int http_arg_as_int(String argName)
{
    http_arg_as_int(argName, 0);
}

int http_arg_as_int(String argName, int defaultValue)
{
    for(uint8_t i = 0; i < HTTPServer.args(); i++)
    {
      if(HTTPServer.argName(i) == argName)
      {
        return HTTPServer.arg(i).toInt();
      }
    }
    return defaultValue;
}

String http_arg_as_string(String argName)
{
  http_arg_as_string(argName);
}

String http_arg_as_string(String argName, String defaultValue = "")
{
    for(uint8_t i = 0; i < HTTPServer.args(); i++)
    {
      if(HTTPServer.argName(i) == argName)
      {
        return HTTPServer.arg(i);
      }
    }
    return defaultValue;
}
