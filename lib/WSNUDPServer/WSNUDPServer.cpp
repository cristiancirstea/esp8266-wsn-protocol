#include "WSNUDPServer.h"
#include "Common.h"

WSNUDPServer* WSNUDPServer::_instance = NULL;

WSNUDPServer* WSNUDPServer::getInstance()
{
  if(_instance == NULL)
  {
    Serial.println("WSN UDP Server created!");
    _instance = new WSNUDPServer;
  }

  return _instance;
}

void WSNUDPServer::handleMessage()
{
  int packetSize = this->parsePacket();

  if(packetSize > 0)
  {
    //WSN::setInternalLedOn();

    char packetBuffer[packetSize + 1];

    // read the packet into packetBufffer
    //int len = Udp.read(packetBuffer, config.udp.packetSize);
    int len = this->read(packetBuffer, packetSize);
    if (len > 0)
    {
      // WSN::logMessage(String("[UDP] Packet of size ") + packetSize + " from ");
      // WSN::logMessage(WSN::ipToString(this->remoteIP()) + ":" + this->remotePort());
      //WSN::logMessage("[UDP] Raw content >>");
      //WSN::logMessage(packetBuffer);

      // packetBuffer[len] = 0; //TODO it is necesary?
      //WSN::logStage("DESERIALIZE PACKET");

      WSNPacket* receivedPacket = WSNPacket::deserialize(packetBuffer, len);
      if(receivedPacket != NULL)
      {
        //Serial.println(receivedPacket->toString());

        //WSN::logStage("END Deserialize Packet");

        //WSN::logStage(String("Handle UDP ") + config.node.getTypeToString());

        switch (config.node.getType())
        {
          case WSN_NODE_TYPE_HEAD_NODE:
            {
              WSNHeadNode* headNode = new WSNHeadNode();
              headNode->handlePacket(receivedPacket);
              delete headNode;
            }
            break;
          case WSN_NODE_TYPE_NODE:
            {
              WSNNode* node = new WSNNode();
              node->handlePacket(receivedPacket);
              delete node;
            }
            break;
          case WSN_NODE_TYPE_SINK:
          //TODO make something
            break;
          default:
            //TODO make something with request message
            break;
        }
        // WSN::logStage("Before delete received package");
        if(receivedPacket != NULL)
        {
          // WSN::logStage("DELETE RECEIVED PACKAGE!");
          delete receivedPacket;
        }
        // WSN::logStage("After delete received package");
      }
      else
      {
        Serial.println("Pachet not deserialized!!!!");
        if(!WSN_PACKET_SERIAL_DEBUG)
        {
          Serial.println("For further info enable WSN_PACKET_SERIAL_DEBUG.");
        }
        if(!WSN_PACKET_ITEM_SERIAL_DEBUG)
        {
          Serial.println("For further info enable WSN_PACKET_ITEM_SERIAL_DEBUG.");
        }
      }
    }
    else
    {
      Serial.println("[UDP] Len = 0 !!!!\n Exit\n");
    }

    //WSN::setInternalLedOff();
  }
}

void WSNUDPServer::sendPacket(WSNPacket* packet, IPAddress ip, unsigned port)
{
  if(packet == NULL)
  {
    return;
  }

  unsigned packetSize = packet->getTotalLengthBytes();
  const char* packetBuffer = packet->serialize();
  if(packetBuffer == NULL)
  {
    Serial.println("[sendPacket] packet can not be serialized.");
    Serial.println("[sendPacket] packet >> ");
    Serial.println(packet->toString());
    return;
  }

  this->beginPacket(ip, port);
  this->write(packetBuffer, packetSize);
  this->endPacket();

  delete packetBuffer;
}

void WSNUDPServer::sendPacket(WSNPacket* packet)
{
    this->sendPacket(packet, config.udp.sendingPort);
}

void WSNUDPServer::sendPacket(WSNPacket* packet, unsigned port)
{
  this->sendPacket(packet, config.wifi.getCredentialsUsed().broadcast, port);
}

void WSNUDPServer::sendPacketSubnet(WSNPacket* packet)
{
  this->sendPacketSubnet(packet, config.udp.sendingPort);
}

void WSNUDPServer::sendPacketSubnet(WSNPacket* packet, unsigned port)
{
  if(!config.wifi.isAPMode())
  {
    return;
  }

  WSN::logMessage(
    String("Sending packet [") + WSNPacket::functionIDToString(packet->getHeader().functionID)  + "] to AP subnet " +
    WSN::ipToString(config.wifi.apBroadcastIP) + ":" + config.udp.sendingPort
  );

  this->sendPacket(packet, config.wifi.apBroadcastIP, config.udp.sendingPort);
}

void WSNUDPServer::sendResponsePacket(WSNPacket* packet)
{
  this->sendPacket(packet, this->remoteIP(), this->remotePort());
}
