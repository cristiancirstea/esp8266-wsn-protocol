#include "WSNSetup.h"
#include "WSNUDPServer.h"
#include "DHTSensor.h"
#include "GY30Sensor.h"
#include "Common.h"

void WSN::setup()
{
  WSN::logStage("SETUP BEGIN");

  WSN::logMessage(String("Network Type: ") + config.network.typeToString());
  setupWiFi();
  setupHTTPServer();
  setupUDPServer();
  setupTickerScheduler();
  setupHeartbeat();
  setupSensors();

  if(config.enabledOTA){
    WSN::setupOTA();
  }

  //TODO use a factory
  if(config.node.getType() == WSN_NODE_TYPE_NODE)
    WSNNode::discover();
  else
    WSNHeadNode::discover();

  WSN::logStage("SETUP END");
}

void setupWiFiSTA()
{
  WSN::scanWiFiAndConnect(true /* forceDisconnect */);
  WSN::logStage("Setup WiFi STA");

  Serial.print("Connected to ");
  Serial.println(config.wifi.getCredentialsUsed().SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupWiFiAP()
{
  WSN::logStage("Setup WiFi AP");
  WiFi.softAP(config.wifi.apSSID, config.wifi.apPassword);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  config.wifi.apIP = WiFi.softAPIP();
}

void setupWiFi()
{
  WSN::logStage("SETUP WIFI");

  WiFi.mode(config.wifi.mode);
  Serial.println("\nSetting esp mode: \""+ config.wifi.modeString() +"\"");

  switch(config.wifi.mode)
  {
    case WIFI_STA:
      setupWiFiSTA();
      //start in AP mode if ther is no WiFi connection established
      if(!config.wifi.isConnected())
      {
        setupWiFiAP();
      }
      break;
    case WIFI_AP:
      setupWiFiAP();
      break;
    case WIFI_AP_STA:
      setupWiFiSTA();
      setupWiFiAP();
      break;
  }
}

void setupUDPServer()
{
  WSN::logStage("SETUP UDP SERVER");
  WSNUDPServer::getInstance()->begin(config.udp.listeningPort);
  config.UDPServerStarted = true;
  Serial.print("UDP Server binded to port: ");
  Serial.println(config.udp.listeningPort);
}

void setupTickerScheduler()
{
  WSN::logStage("TICKER SCHEDULER");

  if(config.sensor.sendDataEnabled  && config.sensor.tickerHandlerID < 0)
  {
    config.sensor.tickerHandlerID = 1;
    config.sensor.callback = sendSensorData;

    if(
      !tickerScheduler.add(
          config.sensor.tickerHandlerID,
          config.sensor.sendDataIntervalMs,
          config.sensor.callback,
          config.sensor.fireOnSetup
        )
    )
    {
      Serial.println("sendSensorData not added to ticker!");
    }
  }
}

void setupHeartbeat()
{
  WSN::logStage("SETUP HEARTBEAT");
  Heartbeat::getInstance()->begin();
}

void setupSensors()
{
  WSN::logStage("SETUP SENSORS");
  Wire.begin();
  if(config.sensor.type == SENSOR_TYPE_DHT_11)
    DHTSensor::setup();
  else if(config.sensor.type == SENSOR_TYPE_GY_30)
    GY30Sensor::setup();
}
