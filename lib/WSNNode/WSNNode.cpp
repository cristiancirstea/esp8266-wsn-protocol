#include "WSNNode.h"
#include "WSNConfig.h"
#include "WSNUDPServer.h"
#include "Common.h"

void WSNNode::handlePacket(WSNPacket* receivedPacket)
{
  WSN::logStage("Node handle package");

  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

  //TODO this should not happend
  if(header.nodeID == config.node.ID)
  {
    WSN::logMessage(String("Received packet from same node ID #") + header.nodeID);
    return;
  }

  switch (header.functionID)
  {
    case WSN::PACKET_FUNCTION_DISCOVER:
      if(flags.messageType == WSN_MESSAGE_TYPE_REQUEST)
      {
        this->_handleDiscoverRequest(receivedPacket);
      }
      else
      {
        this->_handleDiscoverResponse(receivedPacket);
      }
      WSN::ledSignal(header.functionID);
      break;
    case WSN::PACKET_FUNCTION_HEARTBEAT:
      if(flags.messageType == WSN_MESSAGE_TYPE_REQUEST)
      {
        this->_handleHeartbeatRequest(receivedPacket);
      }
      else
      {
        this->_handleHeartbeatResponse(receivedPacket);
      }
      break;
    case WSN::PACKET_FUNCTION_BROADCAST_MESSAGE:
      //TODO
      //no special handle for now
        WSN::ledSignal(header.functionID);
      break;
    case WSN::PACKET_FUNCTION_REGISTER:
      if(flags.messageType == WSN_MESSAGE_TYPE_REQUEST)
      {
        this->_handleRegisterRequest(receivedPacket);
      }
      else
      {
        this->_handleRegisterResponse(receivedPacket);
      }
      break;
    case WSN::PACKET_FUNCTION_QUERY:
      if(flags.messageType == WSN_MESSAGE_TYPE_REQUEST)
      {
        this->_handleQueryRequest(receivedPacket);
      }
      else
      {
        this->_handleQueryResponse(receivedPacket);
      }
      break;
    case WSN::PACKET_FUNCTION_CONFIG:
      if(flags.messageType == WSN_MESSAGE_TYPE_REQUEST)
      {
        this->_handleConfigRequest(receivedPacket);
      }
      else
      {
        this->_handleConfigResponse(receivedPacket);
      }
      break;
    default:
        this->_handleUnknownFunction(receivedPacket);
      break;
  }
}

void WSNNode::discover()
{
  WSN::logStage("SENDING PACKET DISCOVER FROM " + config.node.getTypeToString());
  WSNNodeLinks::clear();
//  WSNChildNodeLinks::clear();// maybe not needed in node

  WSNPacketFlags flags;
  flags.nodeType = config.node.getType(); //or set directly to node
  flags.messageType = WSN_MESSAGE_TYPE_REQUEST;
  flags.messageStatus = WSN_MESSAGE_STATUS_SUCCESS;

  uint8_t ip[4];
  ip[0] = config.wifi.getLocalIP()[0];
  ip[1] = config.wifi.getLocalIP()[1];
  ip[2] = config.wifi.getLocalIP()[2];
  ip[3] = config.wifi.getLocalIP()[3];

  WSNPacket discoverPacket(3);
  discoverPacket.setHeader(config.node.ID, WSN::PACKET_FUNCTION_DISCOVER, 0 /* broadcast */, flags.getValue());
  WSNPacketItem nodeLabel(WSN::PACKET_TAG_NODE_LABEL, config.node.label);
  WSNPacketItem nodeIP(WSN::PACKET_TAG_NODE_IP, ip, 4);
  WSNPacketItem nodePort(WSN::PACKET_TAG_NODE_PORT, config.udp.listeningPort);

  discoverPacket.setItem(0, &nodeLabel);
  discoverPacket.setItem(1, &nodeIP);
  discoverPacket.setItem(2, &nodePort);

  WSNUDPServer::getInstance()->sendPacket(&discoverPacket); /* broadcast */


  WSN::logMessage(String("Discovery Packet broadcasted: \n") + discoverPacket.toString());

  WSN::logStage("END SENDING PACKET DISCOVER FROM " + config.node.getTypeToString());
}

void WSNNode::registerNode(IPAddress addressTo, uint portTo, uint detinationNodeID = 0)
{
  WSN::logStage(String("SENDING PACKET REGISTER FROM ") + config.node.getTypeToString());

  uint8_t ip[4];
  if(WSN::isIPFromSubnet(addressTo))
  {
    ip[0] = config.wifi.getAPIP()[0];
    ip[1] = config.wifi.getAPIP()[1];
    ip[2] = config.wifi.getAPIP()[2];
    ip[3] = config.wifi.getAPIP()[3];
  }
  else
  {
    ip[0] = config.wifi.getLocalIP()[0];
    ip[1] = config.wifi.getLocalIP()[1];
    ip[2] = config.wifi.getLocalIP()[2];
    ip[3] = config.wifi.getLocalIP()[3];
  }

  WSNPacketFlags registerFlags(config.node.getType(), WSN_MESSAGE_TYPE_REQUEST, WSN_MESSAGE_STATUS_SUCCESS);
  WSNPacket registerRequestPacket(12);
  registerRequestPacket.setHeader(config.node.ID, WSN::PACKET_FUNCTION_REGISTER, detinationNodeID, registerFlags.getValue());

  uint8_t wifiModeInt8 = config.wifi.modeInt8();
  int32_t parentID = (config.parent.isSet() ? config.parent.nodeID : 0);


  WSNPacketItem nodeLabel(WSN::PACKET_TAG_NODE_LABEL, config.node.label);
  WSNPacketItem nodeIP(WSN::PACKET_TAG_NODE_IP, ip, 4);
  WSNPacketItem nodePort(WSN::PACKET_TAG_NODE_PORT, config.udp.listeningPort);
  WSNPacketItem sensorID(WSN::PACKET_TAG_SENSOR_ID, config.sensor.ID);
  WSNPacketItem sensorType(WSN::PACKET_TAG_SENSOR_TYPE, config.sensor.type);
  WSNPacketItem apSSID(WSN::PACKET_TAG_NODE_AP_SSID, config.wifi.apSSID);
  WSNPacketItem apPassword(WSN::PACKET_TAG_NODE_AP_PASSWORD, config.wifi.apPassword);
  WSNPacketItem wifiMode(WSN::PACKET_TAG_NODE_WIFI_MODE, &wifiModeInt8, 1);
  WSNPacketItem networkType(WSN::PACKET_TAG_NETWORK_TYPE, &config.network.type, 1);
  WSNPacketItem parentNodeID(WSN::PACKET_TAG_PARENT_NODE_ID, parentID); //Should be overriden by headnode
  WSNPacketItem connectedSSID(WSN::PACKET_TAG_SCAN_WIFI_SSID, config.wifi.getCredentialsUsed().SSID);
  WSNPacketItem connectedSSIDDistance(WSN::PACKET_TAG_SCAN_WIFI_DISTANCE, WSN::distanceFromRSSI(config.wifi.getCredentialsUsed().rssi));


  registerRequestPacket.setItem(0, &nodeLabel);
  registerRequestPacket.setItem(1, &nodeIP);
  registerRequestPacket.setItem(2, &nodePort);
  registerRequestPacket.setItem(3, &sensorID);
  registerRequestPacket.setItem(4, &sensorType); //TODO
  registerRequestPacket.setItem(5, &apSSID);
  registerRequestPacket.setItem(6, &apPassword);
  registerRequestPacket.setItem(7, &wifiMode);
  registerRequestPacket.setItem(8, &networkType);
  registerRequestPacket.setItem(9, &parentNodeID);
  registerRequestPacket.setItem(10, &connectedSSID);
  registerRequestPacket.setItem(11, &connectedSSIDDistance);

  WSNUDPServer::getInstance()->sendPacket(&registerRequestPacket, addressTo, portTo);

  WSN::logMessage(String("To ") + WSN::ipToString(addressTo) + ":" + portTo + " destID = " + detinationNodeID);
  WSN::logMessage(registerRequestPacket.toString());

  WSN::logStage(String("END SENDING PACKET REGISTER FROM ") + config.node.getTypeToString());
}

bool WSNNode::registerToParent()
{
  if(config.parent.isSet())
  {
    WSNNode::registerNode(config.parent.nodeIP, config.parent.nodePort, config.parent.nodeID);
    return true;
  }

  return false;
}

void WSNNode::_addToNodeLinks(WSNPacket* receivedPacket, bool isChild = false)
{
  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

  WSNNodeLink nodeLink;

  nodeLink.setFromPacket(receivedPacket);

  WSN::logMessage(isChild ? "ChildNodeLink >> " : "NodeLink >> ");
  WSN::logMessage(nodeLink.toString());
  if(!isChild)
  {
    WSNNodeLinks::add(&nodeLink, true);
  }
  else
  {
    WSNChildNodeLinks::add(&nodeLink, true);
  }

  // DEBUG
  // this->logAllNodeLinks();
  //END DEBUG
}

void WSNNode::logAllNodeLinks()
{
  WSNNodeLink* nodeLinkIterator;
  WSN::logStage("ALL CHILD NODE LINKS" );
  nodeLinkIterator = WSNChildNodeLinks::getFirst();
  while(nodeLinkIterator != NULL)
  {
    WSN::logMessage(nodeLinkIterator->toString());

    nodeLinkIterator = nodeLinkIterator->next;
  }

  WSN::logStage("END ALL CHILD NODE LINKS");

  WSN::logStage("ALL NODE LINKS");
  nodeLinkIterator = WSNNodeLinks::getFirst();
  while(nodeLinkIterator != NULL)
  {
    WSN::logMessage(nodeLinkIterator->toString());

    nodeLinkIterator = nodeLinkIterator->next;
  }
  WSN::logStage("END ALL NODE LINKS");
}

bool WSNNode::isParentNode(uint8_t nodeType, IPAddress nodeIP)
{
  return nodeType == WSN_NODE_TYPE_SINK
    || (config.network.type == WSN_NETWORK_TYPE_TREE
      && nodeType == WSN_NODE_TYPE_HEAD_NODE
      && nodeIP[3] == 1 /* head node is default gateway */
    )
    || (config.network.type == WSN_NETWORK_TYPE_MESH
      && nodeType == WSN_NODE_TYPE_HEAD_NODE
      && nodeIP[3] == 1 /* head node is default gateway */
    );
}

void WSNNode::_handleDiscoverResponse(WSNPacket* receivedPacket)
{
  WSN::logStage(String("Handle WSN Response Discover ") + config.node.getTypeToString());

  WSN::logMessage("Packet received:");
  WSN::logMessage(receivedPacket->toString());

  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

  if(header.destinationNodeID != config.node.ID)
  {
    WSN::logMessage(String("Destination ID (") + header.destinationNodeID +" ) != config.node.ID (" + config.node.ID + ")");
    return;
  }

  this->_addToNodeLinks(
    receivedPacket,
    WSN::isIPFromSubnet(WSNUDPServer::getInstance()->remoteIP())
  );

  if(this->isParentNode(flags.nodeType, WSNUDPServer::getInstance()->remoteIP()))
  {
    config.parent.setFromPacket(receivedPacket);
    WSNNode::registerToParent();
  }

  WSN::logStage(String("END Handle WSN Response Discover ") + config.node.getTypeToString());
}


void WSNNode::_handleDiscoverRequest(WSNPacket* receivedPacket)
{
  WSN::logStage(String("Handle WSN Request Discover ") + config.node.getTypeToString());
  WSNPacketHeader receivedHeader = receivedPacket->getHeader();
  WSNPacketFlags receivedFlags(receivedHeader.flags);

  //receivedHeader.destinationNodeID must be 0 because is a broadcast

  WSNPacketFlags responseFlags(config.node.getType(), WSN_MESSAGE_TYPE_RESPONSE, WSN_MESSAGE_STATUS_SUCCESS);
  WSNUDPServer* UDPServer = WSNUDPServer::getInstance();

  uint8_t ip[4];
  if(WSN::isIPFromSubnet(UDPServer->remoteIP()))
  {
    ip[0] = config.wifi.getAPIP()[0];
    ip[1] = config.wifi.getAPIP()[1];
    ip[2] = config.wifi.getAPIP()[2];
    ip[3] = config.wifi.getAPIP()[3];
  }
  else
  {
    ip[0] = config.wifi.getLocalIP()[0];
    ip[1] = config.wifi.getLocalIP()[1];
    ip[2] = config.wifi.getLocalIP()[2];
    ip[3] = config.wifi.getLocalIP()[3];
  }

  WSNPacket discoverResponsePacket(3);
  discoverResponsePacket.setHeader(config.node.ID, WSN::PACKET_FUNCTION_DISCOVER, receivedHeader.nodeID, responseFlags.getValue());

  WSNPacketItem nodeLabel(WSN::PACKET_TAG_NODE_LABEL, config.node.label);
  WSNPacketItem nodeIP(WSN::PACKET_TAG_NODE_IP, ip, 4);
  WSNPacketItem nodePort(WSN::PACKET_TAG_NODE_PORT, config.udp.listeningPort);

  discoverResponsePacket.setItem(0, &nodeLabel);
  discoverResponsePacket.setItem(1, &nodeIP);
  discoverResponsePacket.setItem(2, &nodePort);

  UDPServer->sendResponsePacket(&discoverResponsePacket);

  this->_addToNodeLinks(
    receivedPacket,
    WSN::isIPFromSubnet(UDPServer->remoteIP())
  );

  WSN::logMessage("<< Response Packet >>");
  WSN::logMessage(discoverResponsePacket.toString());

  if(this->isParentNode(receivedFlags.nodeType, UDPServer->remoteIP()))
  {
    config.parent.setFromPacket(receivedPacket);
    WSNNode::registerToParent();
  }

  WSN::logStage(String("END Handle WSN Request Discover ") + config.node.getTypeToString());
}

void WSNNode::_handleRegisterRequest(WSNPacket* receivedPacket)
{
  WSN::logStage("!!! Register Request Node !!!");

  WSN::logMessage("Ignoring this kind of message. This should not arrive here. Check network configuration!");
  WSN::logMessage("Received packet:");
  WSN::logMessage(receivedPacket->toString());

  WSN::logStage("!!! End Register Request Node !!!");
}

void WSNNode::_handleRegisterResponse(WSNPacket* receivedPacket)
{
  WSN::logStage("Register Response NODE");


  //TODO set config if response has attributes with different values
  WSN::logMessage(receivedPacket->toString());

  WSN::logStage("END Register Response NODE");
}


void WSNNode::_handleQueryRequest(WSNPacket* receivedPacket)
{
  WSN::logStage("QueryRequest NODE");
  Serial.println("Request packet >> ");
  Serial.println(receivedPacket->toString());

  if(receivedPacket->getItemCount() > 0)
  {
    WSN::logStage(String("Query with (") + receivedPacket->getItemCount() + ") attributes.");

    WSNPacket* responsePacket = new WSNPacket(receivedPacket->getItemCount());
    WSNPacketFlags flags(config.node.getType(), WSN_MESSAGE_TYPE_RESPONSE, WSN_MESSAGE_STATUS_SUCCESS);
    responsePacket->setHeader(config.node.ID, WSN::PACKET_FUNCTION_QUERY, receivedPacket->getHeader().nodeID, flags.getValue());

    bool mustRespond = true;

    for(int i = 0; i < receivedPacket->getItemCount(); i++)
    {
      //TODO clone this not get by reference!!!
      WSNPacketItem* packetItem = receivedPacket->getItem(i);
      if(packetItem)
      {
        WSN::logStage(String("Packet Item #") + i + " >> ");
        WSN::logMessage(packetItem->toString());


        if(packetItem->isValueNull())
        {
          switch (packetItem->getTag()) {
            case WSN::PACKET_TAG_NODE_ID:
                packetItem->setValue(config.node.ID);
              break;
            case WSN::PACKET_TAG_NODE_LABEL:
                packetItem->setValue(config.node.label);
              break;
            case WSN::PACKET_TAG_NODE_TYPE:
              {
                uint8_t nodeType = config.node.getType();
                packetItem->setValue(&nodeType, nodeType);
              }
              break;
            case WSN::PACKET_TAG_SENSOR_ID:
                packetItem->setValue(config.sensor.ID);
              break;
            case WSN::PACKET_TAG_SENSOR_VALUE:
                packetItem->setValue(WSN::getSensorData().value);
              break;
            case WSN::PACKET_TAG_SENSOR_LABEL:
                char sensorLabel[50];
                config.sensor.label.toCharArray(sensorLabel, 50);
                packetItem->setValue(sensorLabel);
              break;
            case WSN::PACKET_TAG_NETWORK_TYPE:
                packetItem->setValue(&config.network.type, 1);
              break;
            default:
              packetItem->setValueNull();
              break;
          }

          responsePacket->setItem(i, packetItem);
          WSN::logMessage(String("NULL ") + WSNPacketItem::tagToString(packetItem->getTag()) +
            ". Setting value from config.");
        }
        else
        {
          if(!this->_checkQueryItemValue(packetItem))
          {
            WSN::logMessage(WSNPacketItem::tagToString(packetItem->getTag()) + " is different than this node value.");
            mustRespond = false;
            break;
          }
          else
          {
            responsePacket->setItem(i, packetItem);
            WSN::logMessage(String("Same value for ") + WSNPacketItem::tagToString(packetItem->getTag()));
          }
        }
      }
    }

    if(mustRespond)
    {
      WSN::logStage("Sending response packet");
      WSN::logMessage(responsePacket->toString());
      WSNUDPServer::getInstance()->sendResponsePacket(responsePacket);
      WSN::ledSignal(2);
    }
    else
    {
      WSN::logMessage("Must not respond.");
    }

  }
  else
  {
    WSN::logMessage("Query with no attributes. Nothing to do...");
  }

  WSN::logStage("END QueryRequest NODE");
}

void WSNNode::_handleQueryResponse(WSNPacket* receivedPacket)
{
  WSN::logStage("QueryResponse NODE");

  Serial.println(receivedPacket->toString());

  WSN::logStage("END QueryResponse NODE");
}

bool WSNNode::_checkQueryItemValue(WSNPacketItem* packetItem)
{
  switch (packetItem->getTag()) {
    case WSN::PACKET_TAG_NODE_ID:
        return (*(uint32_t*)packetItem->getValue() == config.node.ID);
      break;
    case WSN::PACKET_TAG_NODE_TYPE:
        return (*(uint8_t*)packetItem->getValue() == config.node.getType());
      break;
    case WSN::PACKET_TAG_NETWORK_TYPE:
        return (*(uint8_t*)packetItem->getValue() == config.network.type);
      break;
    case WSN::PACKET_TAG_NODE_LABEL:
        return true; //TODO
      break;
    case WSN::PACKET_TAG_SENSOR_ID:
        return (*(uint32_t*)packetItem->getValue() == config.sensor.ID); //TODO check if this value is in sensor list
      break;
    case WSN::PACKET_TAG_SENSOR_VALUE:
        return true; //TODO
      break;
    case WSN::PACKET_TAG_SENSOR_LABEL:
        return true; //TODO
      break;
    default:
      return false;
  }

  return false;
}

void WSNNode::_handleConfigRequest(WSNPacket* receivedPacket)
{
  WSN::logStage("ConfigRequest NODE");
  WSN::logMessage("Received Message:");
  Serial.println(receivedPacket->toString());

  bool mustRestart = false;

  if(receivedPacket->getItemCount() > 0)
  {
    WSN::logStage(String("Config with (") + receivedPacket->getItemCount() + ") attributes.");

    for(int i = 0; i < receivedPacket->getItemCount(); i++)
    {
      //TODO clone this not get by reference!!!
      WSNPacketItem* packetItem = receivedPacket->getItem(i);
      if(packetItem)
      {
        WSN::logStage(String("Packet Item #") + i + " >> ");
        WSN::logMessage(packetItem->toString());


        if(packetItem->isValueNull())
        {
          WSN::logMessage("NULL value. Ignoring...");
        }
        else
        {
          switch (packetItem->getTag()) {
            case WSN::PACKET_TAG_NODE_ID:
                Serial.println(String("Node ID: ") + packetItem->valueToString());
                config.node.ID = packetItem->getValueUInt16();
                mustRestart = true;
              break;
            case WSN::PACKET_TAG_NODE_LABEL:
                Serial.println(String("Node Label: ") + packetItem->valueToString());
                packetItem->valueToString().toCharArray(config.node.label, 50);
                  mustRestart = true;
              break;
            case WSN::PACKET_TAG_NODE_TYPE:
              {
                Serial.println(String("Node Type: ") + packetItem->valueToString());
                config.node.setType(packetItem->getValueUInt8());
                  mustRestart = true;
              }
              break;
            case WSN::PACKET_TAG_SENSOR_ID:

              break;
            case WSN::PACKET_TAG_SENSOR_VALUE:

              break;
            case WSN::PACKET_TAG_SENSOR_LABEL:

              break;
            case WSN::PACKET_TAG_NETWORK_TYPE:
                Serial.println(String("Node Label: ") + packetItem->valueToString());
                config.network.type = packetItem->getValueUInt8();
                  mustRestart = true;
              break;
            default:

              break;
          }
        }
      }
      else{
        WSN::logMessage("[ERROR] NULL pachet Item!!!");
      }
    }

  }
  else
  {
    WSN::logMessage("Config with no attributes. Nothing to do...");
  }

  if(mustRestart)
  {
      WSN::logMessage("Restarting from Config...");
      //TODO restart
      setup();
  }

  WSN::logStage("END ConfigRequest NODE");
}

void WSNNode::_handleConfigResponse(WSNPacket* receivedPacket)
{
  WSN::logStage("ConfigyResponse NODE");

  Serial.println(receivedPacket->toString());

  WSN::logStage("END ConfigyResponse NODE");
}

void WSNNode::_handleHeartbeatRequest(WSNPacket* receivedPacket)
{

}

void WSNNode::_handleHeartbeatResponse(WSNPacket* receivedPacket)
{
  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);
  if(flags.nodeType == WSN_NODE_TYPE_SINK)
  {
    WSN::ledSignal(header.functionID);
  }
}

void WSNNode::_handleUnknownFunction(WSNPacket* receivedPacket)
{
  //TODO
}
