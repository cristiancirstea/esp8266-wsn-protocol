#include "WSNOTA.h"

void WSN::setupOTA(){
  WSN::logStage("SETUP OTA");
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("[OTA] Started.");
    WSN::ledSignal(5, 50);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("[OTA] End.");
    WSN::ledSignal(5, 50);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  WSN::logMessage(String("OTA Ready\nOver The Air Upload to ") + WSN::ipToString(config.wifi.getLocalIP()));

  WSN::logStage("END OTA SETUP");
}

void WSN::handleOTA(){
  ArduinoOTA.handle();
}
