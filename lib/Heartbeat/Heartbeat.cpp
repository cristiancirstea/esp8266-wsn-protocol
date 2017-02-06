#include "Heartbeat.h"

#include "Common.h"

Heartbeat* Heartbeat::_instance = NULL;
bool Heartbeat::_instanceCreated = false;

void heartbeat_send()
{
  // Serial.println("------------ Heartbeat from ticker -------------------");
  Heartbeat* instance = Heartbeat::getInstance();
  instance->send();
}

Heartbeat::Heartbeat()
{

}

void Heartbeat::begin()
{
  Serial.println("Begin Heartbeat...");
  config.heartbeat.tickerHandlerID = 0;
  config.heartbeat.callback = heartbeat_send;

  this->_tickerScheduler.init(1);
  if(
      !this->_tickerScheduler.add(
        config.heartbeat.tickerHandlerID,
        config.heartbeat.intervalMs,
        config.heartbeat.callback,
        config.heartbeat.fireOnSetup
      )
  )
  {
      config.heartbeat.tickerHandlerID = -1;
      Serial.println("Heartbeat ticker was not set!");
  }
}

void Heartbeat::end()
{
  this->_tickerScheduler.remove(config.heartbeat.tickerHandlerID);
  config.heartbeat.tickerHandlerID = -1;
}

void Heartbeat::update()
{
  if(!config.heartbeat.enabled && config.heartbeat.tickerHandlerID >= 0)
  {
    Serial.println("END Heartbeat...");
    this->end();
  }
  this->_tickerScheduler.update();
}

//TODO may not be needed
void Heartbeat::_sendUDP(char UDPHeartbeatBuffer[], bool redirected)
{
  WiFiUDP Udp;
  if(config.heartbeat.enableLog)
  {
    if(!redirected)
    {
      Serial.println("------------------SEND HEARTBEAT----------------");
    }
    else
    {
      Serial.println("-----------------REDIRECT HEARTBEAT------------");
    }
  }

    if(config.heartbeat.enableLog)
    {
      char s[16];
      sprintf(
        s,
        "%d.%d.%d.%d",
        config.wifi.getCredentialsUsed().broadcast[0],
        config.wifi.getCredentialsUsed().broadcast[1],
        config.wifi.getCredentialsUsed().broadcast[2],
        config.wifi.getCredentialsUsed().broadcast[3]
      );
      Serial.println(String("Sending UDP Heartbeat to: ") + s + ":" + config.heartbeat.UDPPort + "...");
    }

    Udp.beginPacket(config.wifi.getCredentialsUsed().broadcast, config.heartbeat.UDPPort);
    Udp.write(UDPHeartbeatBuffer);
    Udp.endPacket();

    //TODO send in subnet if AP
}


void Heartbeat::_sendPacket(WSNPacket* packet)
{
  WSNUDPServer* Udp = WSNUDPServer::getInstance();

  if(config.heartbeat.enableLog)
  {
    Serial.println("Sending heartbeat message >>" + packet->toString());
  }

  Udp->sendPacket(packet, config.wifi.getCredentialsUsed().broadcast, config.heartbeat.UDPPort);

  if(config.heartbeat.sendToAP)
  {
    Udp->sendPacketSubnet(packet, config.heartbeat.UDPPort);
  }

  if(config.heartbeat.enableLog)
  {
    Serial.printf(
      "Heartbeat packet sent to %d.%d.%d.%d:%d and AP subnet %s:%d\n",
      config.wifi.getCredentialsUsed().broadcast[0],
      config.wifi.getCredentialsUsed().broadcast[1],
      config.wifi.getCredentialsUsed().broadcast[2],
      config.wifi.getCredentialsUsed().broadcast[3],
      config.heartbeat.UDPPort,
      "192.168.4.255", //TODO
      config.heartbeat.UDPPort
    );
    Serial.println("---------------------------------\n");
  }
}

void Heartbeat::_sendHTTP(int nodeID)
{
  Serial.print("HTTP heartbeat for node #");
  Serial.println(nodeID);
  WSNClient client(String(config.sink.APIHost), config.sink.APIPort, nodeID);

  client.disableWaitForRespose();
  /*String response =*/ client.getRequest("/heartbeat"); //TODO node id in header
}

void Heartbeat::_sendHTTP()
{
  _sendHTTP(config.node.ID);
}

//TODO may not be nedded
void Heartbeat::sendMessage(char UDPHeartbeatBuffer[], bool redirected)
{

  if(config.heartbeat.mode == HEARTBEAT_MODE_UDP || config.heartbeat.mode == HEARTBEAT_MODE_UDP_HTTP)
  {
    this->_sendUDP(UDPHeartbeatBuffer, redirected);
  }

  if(config.heartbeat.mode == HEARTBEAT_MODE_HTTP || config.heartbeat.mode == HEARTBEAT_MODE_UDP_HTTP)
  {
    if(!redirected)
    {
      this->_sendHTTP();
    }
    else
    {
      //TODO get actual nodeID
      this->_sendHTTP(2);
    }

  }
}

void Heartbeat::send()
{
  //TODO send more data -> sensor value

    WSNPacketFlags flags(config.node.getType(), WSN_MESSAGE_TYPE_RESPONSE, WSN_MESSAGE_STATUS_SUCCESS);

    WSNPacket packetHeartbeat(3);
    packetHeartbeat.setHeader(config.node.ID, WSN::PACKET_FUNCTION_HEARTBEAT, 0, flags.getValue());
    //WSNPacketItem nodeID(WSN::PACKET_TAG_NODE_ID, config.node.ID);
    //packetHeartbeat.setItem(0, &nodeID);
    WSNSensorValue sensorData = WSN::getSensorData();
    WSNPacketItem sensorValue(WSN::PACKET_TAG_SENSOR_VALUE, sensorData.value);
    WSNPacketItem sensorType(WSN::PACKET_TAG_SENSOR_TYPE, config.sensor.type);
    WSNPacketItem sensorID(WSN::PACKET_TAG_SENSOR_ID, config.sensor.ID);

    packetHeartbeat.setItem(0, &sensorValue);
    packetHeartbeat.setItem(1, &sensorType);
    packetHeartbeat.setItem(2, &sensorID);

    this->_sendPacket(&packetHeartbeat);

    if(config.heartbeat.mode == HEARTBEAT_MODE_HTTP || config.heartbeat.mode == HEARTBEAT_MODE_UDP_HTTP)
    {
      this->_sendHTTP();
    }
}
