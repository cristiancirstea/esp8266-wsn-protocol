// #include "HeadNode.h"
// #include "Node2.h"
#include "Node3.h"
// #include "Node4.h"
// #include "Node5.h"
// #include "Node6.h"

#include <WSNConfig.h>
#include <TickerScheduler.h>
#include <Heartbeat.h>
#include <WSNSetup.h>
#include <Common.h>
#include <WSNOTA.h>
#include <OLED.h>

// WSNConfig config;

// TickerScheduler tickerScheduler(config.ticker.callbackCount);

/******************* SETUP **********************/
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting...");
  // oled_setup();

  // pinMode(A0, INPUT);
  // pinMode(config.internalLedPin, OUTPUT);

  // WSN::setInternalLedOff();

  // WSN::setup();

  // oled_print(String("Node \n#") + config.node.ID);
}

/********************* LOOP *********************/
void loop()
{
  // if(config.HTTPServerStarted)
  // {
  //   HTTPServer.handleClient();
  // }
  // if(config.UDPServerStarted)
  // {
  //   WSNUDPServer::getInstance()->handleMessage();
  // }
  // if(!config.heartbeat.enabled && config.heartbeat.tickerHandlerID >= 0)
  // {
  //   Heartbeat::getInstance()->end();
  // }

  // if(!config.sensor.sendDataEnabled && config.sensor.tickerHandlerID >= 0)
  // {
  //   tickerScheduler.remove(config.sensor.tickerHandlerID);
  //   config.sensor.tickerHandlerID = -1;
  // }

  // tickerScheduler.update();
  // Heartbeat::getInstance()->update();
  // WSN::handleOTA();
}
