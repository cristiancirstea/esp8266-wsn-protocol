#ifndef WSN_NODE_LINK_H
#define WSN_NODE_LINK_H

#include <Arduino.h>
#include <IPAddress.h>
#include "WSNPacket.h"

struct WSNNodeLink{
  unsigned nodeID = 0;
  String nodeLabel; //TODO
  uint8_t nodeType = WSN_NODE_TYPE_NODE;
  IPAddress nodeIP;
  uint16_t nodePort = 0;
  WSNNodeLink* next = NULL;

  bool isSet()
  {
    return nodeID > 0;
  }

  bool hasNext()
  {
    return next != NULL;
  }

  void set(unsigned nodeID, String nodeLabel, uint8_t nodeType, IPAddress nodeIP, uint16_t nodePort)
  {
      this->nodeID = nodeID;
      this->nodeLabel = nodeLabel;
      if(nodeType == WSN_NODE_TYPE_NODE
        || nodeType == WSN_NODE_TYPE_HEAD_NODE
        || nodeType == WSN_NODE_TYPE_SINK
      )
      {
        this->nodeType = nodeType;
      }
      this->nodeIP = nodeIP;
      this->nodePort = nodePort;
  }

  void setFromPacket(WSNPacket* packet)
  {
    WSNPacketHeader header = packet->getHeader();
    WSNPacketFlags flags(header.flags);

    this->nodeID = header.nodeID;
    WSNPacketItem* nodeLabel = packet->getItemByTag(WSN::PACKET_TAG_NODE_LABEL);
    if(nodeLabel != NULL)
    {
      this->nodeLabel = String(nodeLabel->getValue());
    }
    this->nodeType = flags.nodeType;
    WSNPacketItem* nodeIP = packet->getItemByTag(WSN::PACKET_TAG_NODE_IP);
    if(nodeIP != NULL)
    {
      uint8_t* ip = (uint8_t*)nodeIP->getValue();
      this->nodeIP = IPAddress(ip);
    }
    else
    {
      this->nodeIP = IPAddress(0, 0, 0, 0);
    }
    WSNPacketItem* nodePort = packet->getItemByTag(WSN::PACKET_TAG_NODE_PORT);
    if(nodePort != NULL)
    {
      uint16_t* port = (uint16_t*)nodePort->getValue();
      this->nodePort = *port;
    }
    else
    {
      this->nodePort = 0;
    }
  }

  String toString()
  {
    return String("nodeID: ") + nodeID + "\n" +
      "nodeLabel: " + nodeLabel + "\n" +
      "nodeType: " + nodeType + "\n" +
      "nodeIP: " + nodeIP[0] + "." + nodeIP[1] + "." +nodeIP[2] + "." +nodeIP[3] + "\n" +
      "nodePort: " + nodePort + "\n";
  }
};


#endif //WSN_NODE_LINK_H
