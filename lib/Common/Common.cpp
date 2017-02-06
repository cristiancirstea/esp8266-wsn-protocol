#include "Common.h"

void WSN::setInternalLedOn()
{
  digitalWrite(config.internalLedPin, LOW);
}

void WSN::setInternalLedOff()
{
  digitalWrite(config.internalLedPin, HIGH);
}

void WSN::logMessage(IPAddress ip)
{
  logMessage(ipToString(ip));
}

void WSN::logMessage(String message)
{
  if(!config.DEBUG)
  {
    return;
  }
  Serial.println(message);
}

void WSN::logStage(String stage)
{

  if(!config.DEBUG)
  {
    return;
  }
  Serial.print("------------------ ");
  Serial.print(stage);
  Serial.println(" ------------------");
}

//TODO move this
WSNSensorValue WSN::getSensorData()
{
  //TODO
  if(!config.sensor.enabled)
  {
    return WSNSensorValue(0, "Undefined", "Undefined");
  }
  switch (config.sensor.type)
  {
    case SENSOR_TYPE_TMP_102:
      return WSNSensorValue(WSN::getTMP102Value(config.sensor.i2cAddress), "Temperature", "\u00B0C");
    case SENSOR_TYPE_DHT_11:
      return DHTSensor::value();
    case SENSOR_TYPE_GY_30:
      return GY30Sensor::value();
    default:
      return WSNSensorValue(random(10, 100) * 1.0 / random(1, 10), "Undefined", "Undefined");
  }
}

String WSN::ipToString(IPAddress ip)
{
  char s[16];
  sprintf(s, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return String(s);
}

/**
* TODO calibrate this better
*http://electronics.stackexchange.com/questions/83354/calculate-distance-from-rssi
*/
float WSN::distanceFromRSSI(double rssi)
{
  double n = 2.2;
  double A = -44;

  float d = pow(10, ((rssi - A )/((-10) * n)));

  return d;
}

/****************** TEMPERATURE ******************/
float WSN::getTMP102Value(int tmp102Address)
{
  Wire.requestFrom(tmp102Address, 2); // We use our wire library’s get two readings from our TMP102. We pass it the address and the quantity.

  //You may have noticed that we didn’t do anything with the request. That’s because we have to read back we requested. We do that sequentially.
  byte MSB = Wire.read(); // The first byte we get, the Most Significant, get’s stored in MSB.
  byte LSB = Wire.read(); // The second byte we get, the Least Significant, get’s stored in LSB.

  int TemperatureSum = ((MSB << 8) | LSB) >> 4;

  float celsius = TemperatureSum * 0.0625; // Convert our temp to celsius.


  return celsius; // Return our result to be printed.
}

//assumes little endian
void WSN::printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            Serial.printf("%u", byte);
        }
        Serial.print(" ");
    }
    Serial.println("");
}

 bool WSN::isIPFromSubnet(IPAddress theIP)
 {
   if(!config.wifi.isAPMode())
   {
     return false;
   }

   return (theIP[2] == config.wifi.getAPIP()[2]);
 }

void WSN::ledSignal(uint8_t signalCount)
{
  WSN::ledSignal(signalCount, NODE_LED_SIGNAL_DELAY);
}

void WSN::ledSignal(uint8_t signalCount, unsigned long delayMilis)
{
  for(uint8_t i = 0; i < signalCount; i++)
  {
    delay(delayMilis);
    setInternalLedOn();
    delay(delayMilis);
    setInternalLedOff();
  }
}


WSNUDPServer* WSN::UDPServer()
{
  return  WSNUDPServer::getInstance();
}



/**************** HTTP CLIENT ****************/
void sendSensorData()
{
  WSN::logStage("SEND SENSOR DATA");

  WSNClient client(config.sink.APIHost, config.sink.APIPort, config.node.ID);
  // float value = WSN::getSensorData();
  int valueA =  analogRead(A0);
  float value = valueA * 4.2 / 1024;

  // if(abs(value - config.sensor.lastValue) < config.sensor.threshold)
  // {
  //   Serial.println("Sensor value does not exceed the threshold from the previous value.");
  //   Serial.print("Current value: ");
  //   Serial.println(value);
  //   Serial.print("Last value: ");
  //   Serial.println(config.sensor.lastValue);
  //   return;
  // }

//TODO
  if(value >= 150.0)
  {
    Serial.printf("Value %f is greater than maximum permitted %f.", value, 150.0);
    return;
  }

  String unit = config.sensor.unit;
  String message = "{";
  message += String(" \"value\": \"") + String(value) + "\",";
  message += String(" \"lastValue\":\" ") + String(config.sensor.lastValue) + "\",";
  message += String("\"unit\":\"") + /*config.sensor.unit*/ "V" + "\", ";
  message += String("\"label\":\"") + "battery"/*config.sensor.label*/ + "\", ";
  message += String("\"nodeID\" : ") + String(config.node.ID);
  message += "}";

  client.disableWaitForRespose();
 /* String response = */ client.postRequest("/sensor", message);
 // Serial.print("Response: ");
 // Serial.println(response);

 config.sensor.lastValue = value;
}

/************* WIFI ***********************/
bool WSN::connectToWiFi(WiFiCredentials wifiCredentials)
{
    int nConnectRetries = 0;
    Serial.print("Connecting to ");
    Serial.println(wifiCredentials.SSID);

    WiFi.begin(wifiCredentials.SSID, wifiCredentials.password);
    if(wifiCredentials.useStaticIP)
    {
      Serial.print("Using Static IP ");
      Serial.print(WSN::ipToString(wifiCredentials.staticIP));
      Serial.println(" ...");
      WiFi.config(wifiCredentials.staticIP, wifiCredentials.gateway, wifiCredentials.mask);
    }

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(WIFI_CONNECTION_RETRIES_DELAY);
      Serial.print(".");

      nConnectRetries++;
      if(nConnectRetries >= WIFI_CONNECTION_RETRIES_MAX)
      {
        Serial.print("Failed to connect to ");
        Serial.println(wifiCredentials.SSID);
        nConnectRetries = 0;
        break;
      }
    }
    Serial.println("");

    if(WiFi.status() == WL_CONNECTED)
    {
      return true;
    }

    return false;
}

void WSN::scanWiFiAndConnect(bool forceDisconnect = false)
{
  WiFiCredentials* networksFound;

  int availableNetworks = WSN::scanWiFiNetworks(networksFound);

  if(forceDisconnect && config.wifi.isConnected())
  {
    Serial.printf("Disconnecting from %s \n", config.wifi.getCredentialsUsed().SSID);
    WiFi.disconnect(false);
    config.wifi.setDisconnected();
  }

  if(!config.wifi.isConnected())
  {
    //TODO use some logic here!!! based on node type
    for(int networkIndex = 0; networkIndex < availableNetworks; networkIndex++)
    {
      if(
        config.node.getType() == WSN_NODE_TYPE_NODE
        && (config.network.type == WSN_NETWORK_TYPE_TREE
        /* || config.network.type == WSN_NETWORK_TYPE_MESH*/)
      )
      {
        if(networksFound[networkIndex].isFromHeadNode())
        {
          networksFound[networkIndex].setPassword(config.wifi.apPassword);
          networksFound[networkIndex].useStaticIP = true;
          networksFound[networkIndex].staticIP = IPAddress(192,168,4, config.wifi.subnetSufix);
          networksFound[networkIndex].gateway = IPAddress(192,168,4,1);
          networksFound[networkIndex].mask = IPAddress(255,255,255,0);
          networksFound[networkIndex].broadcast = IPAddress(192,168,4,255);
          if(WSN::connectToWiFi(networksFound[networkIndex]))
          {
            //TODO
            config.wifi.credentials[WIFI_CREDENTIALS_MAX_COUNT - 1] = networksFound[networkIndex];

            config.wifi.setCredentialsUsed(WIFI_CREDENTIALS_MAX_COUNT - 1);
            delete[] networksFound;
            return;
            break;
          }
        }
      }

      //putt signal strength/rssi on all known WiFiCredentials
      for(int i = 0; i < config.wifi.credentialsCount; i++)
      {
        if(String(config.wifi.credentials[i].SSID) == networksFound[networkIndex].SSID)
        {
          config.wifi.credentials[i].rssi = networksFound[networkIndex].rssi;
          break;
        }
      }
    }

    Serial.println("------ Known SSIDs -----");
    for(int i=0; i < config.wifi.credentialsCount; i++)
    {
      Serial.print(config.wifi.credentials[i].SSID);
      Serial.print(" -> ");
      Serial.println(String(config.wifi.credentials[i].rssi) + "db");
    }


    //iterate from last to preserve priority
    for(int i = config.wifi.credentialsCount - 1; i >= 0; i--)
    {
      //Prioritize order from config and skip SSID with no signal
      if(config.wifi.credentials[i].rssi <= WIFI_NO_SIGNAL_RSSI)
      {
        continue;
      }
      if(WSN::connectToWiFi(config.wifi.credentials[i]))
      {
        config.wifi.setCredentialsUsed(i);
        break;
      }
    }
  }

  delete[] networksFound;
}

int WSN::scanWiFiNetworks(WiFiCredentials* &networksFound)
{
  WSN::logStage("Scan WiFi");

  byte available_networks = WiFi.scanNetworks();
  networksFound = new WiFiCredentials[available_networks];

  WiFiCredentials aux;

  Serial.printf("------ Networks found (%d)-----\n", available_networks);
  for(int networkIndex = 0; networkIndex < available_networks; networkIndex++)
  {
    String ssidFound = WiFi.SSID(networkIndex);
    int32_t rssi = WiFi.RSSI(networkIndex);
  //  Serial.println(ssidFound + ": " + rssi + "dBm ~" + WSN::distanceFromRSSI(rssi) + "m");

    char ssid[32];
    ssidFound.toCharArray(ssid, ssidFound.length() + 1);
    aux.setSSID(ssid);
    aux.rssi = rssi;

    for(int iterator = 0; iterator < available_networks; iterator++)
    {
        if(networksFound[iterator].rssi < aux.rssi)
        {
          char ssidTemp[32];

          snprintf(ssidTemp, 32, "%s", aux.SSID);
          int32_t rssiTemp = aux.rssi;

          aux = networksFound[iterator];

          networksFound[iterator].setSSID(ssidTemp);
          networksFound[iterator].rssi = rssiTemp;
        }
        else if(aux.rssi == WIFI_NO_SIGNAL_RSSI)
        {
          break;
        }
    }
  }
  Serial.println("------ Ordered SSIDs by RSSI-----");
  for(int i = 0; i < available_networks; i++)
  {
    Serial.printf("%s : %d dBm ~", networksFound[i].SSID, networksFound[i].rssi);
    Serial.println(String(WSN::distanceFromRSSI(networksFound[i].rssi)) + " m");
  }

  return available_networks;
}

void WSN::sendHTTPToIFTTT()
{
  //TEST
  String mailTo = "cristiancocioaba@gmail.com";
  // String mailTo = "marian.vasilca@gmail.com";
  WSNClient ifttt(String("maker.ifttt.com"), 80, config.node.ID);
  // ifttt.disableWaitForRespose();
  ifttt.postRequest(
    String("/trigger/wsn_test/with/key/mHXKyMv2h2GzbMdFj9XsgUm-UHIDKhaKew88NP5iVev"),
    String("{ \"value1\" : \"Node ID: ") + config.node.ID + "\nNode Label" + config.node.label +"\"," +
      " \"value2\" : \"Sensor Value: 123\"," +
      " \"value3\" : \"" + mailTo + "\" }"
  );
}
