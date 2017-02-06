#include "WSNHeadNode.h"
#include "Common.h"

void WSNHeadNode::handlePacket(WSNPacket* receivedPacket)
{
  // WSN::logStage("HeadNode handle package");

  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

  //TODO this should not happend
  if(header.nodeID == config.node.ID)
  {
    Serial.println(String("Received packet from same node ID #") + header.nodeID);
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
      if(
        flags.messageType == WSN_MESSAGE_TYPE_REQUEST
        && flags.nodeType == WSN_NODE_TYPE_SINK
      )
      {
        WSNPacketItem* customMessage = receivedPacket->getItemByTag(WSN::PACKET_TAG_CUSTOM_MESSAGE);
        if(customMessage != NULL)
        {
          WSN::logStage("Custom Broadcast message:");
          Serial.println(customMessage->valueToString());
        }
        WSNUDPServer::getInstance()->sendPacketSubnet(receivedPacket);
      }

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

void WSNHeadNode::discover()
{
  WSN::logStage("SENDING Head Node PACKET DISCOVER");
  WSNChildNodeLinks::clear();

  //Send discover to connected network (STA)
  WSNNode::discover();

  //TODO may be needed to send this at an interval after setup!!!
  // So that the nodes can be connected before sending this packet
  //Head Node sends also to subnet with apIP NOT localIP
  WSNPacketFlags flags;
  flags.nodeType = config.node.getType(); //or set directly to headnode
  flags.messageType = WSN_MESSAGE_TYPE_REQUEST;
  flags.messageStatus = WSN_MESSAGE_STATUS_SUCCESS;

  uint8_t apIP[4];
  apIP[0] = config.wifi.getAPIP()[0];
  apIP[1] = config.wifi.getAPIP()[1];
  apIP[2] = config.wifi.getAPIP()[2];
  apIP[3] = config.wifi.getAPIP()[3];

  WSNPacket discoverPacketSubnet(3);
  discoverPacketSubnet.setHeader(config.node.ID, WSN::PACKET_FUNCTION_DISCOVER, 0, flags.getValue());
  WSNPacketItem nodeLabel(WSN::PACKET_TAG_NODE_LABEL, config.node.label);
  WSNPacketItem nodeAPIP(WSN::PACKET_TAG_NODE_IP, apIP, 4);
  WSNPacketItem nodePort(WSN::PACKET_TAG_NODE_PORT, config.udp.listeningPort);

  discoverPacketSubnet.setItem(0, &nodeLabel);
  discoverPacketSubnet.setItem(1, &nodeAPIP);
  discoverPacketSubnet.setItem(2, &nodePort);

  WSNUDPServer::getInstance()->sendPacketSubnet(&discoverPacketSubnet);

  WSN::logMessage(String("Discover Packet To Subnet: \n") + discoverPacketSubnet.toString());

  WSN::logStage("END SENDING Head Node PACKET DISCOVER");
}


//TODO remove this method or add extra functionality
bool WSNHeadNode::isParentNode(uint8_t nodeType, IPAddress nodeIP)
{
  return WSNNode::isParentNode(nodeType, nodeIP);
}


//TODO remove this method or add extra functionality
void WSNHeadNode::_handleDiscoverResponse(WSNPacket* receivedPacket)
{
  WSNNode::_handleDiscoverResponse(receivedPacket);
}


void WSNHeadNode::_handleDiscoverRequest(WSNPacket* receivedPacket)
{
  WSN::logStage("Handle WSN Request Discover");
  WSNPacketHeader receivedHeader = receivedPacket->getHeader();
  WSNPacketFlags receivedFlags(receivedHeader.flags);

  WSNPacketFlags flags(config.node.getType(), WSN_MESSAGE_TYPE_RESPONSE, WSN_MESSAGE_STATUS_SUCCESS);
  WSNUDPServer* UDPServer = WSNUDPServer::getInstance();

  uint8_t ip[4];
  ip[0] = config.wifi.getLocalIP()[0];
  ip[1] = config.wifi.getLocalIP()[1];
  ip[2] = config.wifi.getLocalIP()[2];
  ip[3] = config.wifi.getLocalIP()[3];

  WSNPacket discoverResponsePacket(3);
  discoverResponsePacket.setHeader(config.node.ID, WSN::PACKET_FUNCTION_DISCOVER, receivedHeader.nodeID, flags.getValue());

  WSNPacketItem nodeLabel(WSN::PACKET_TAG_NODE_LABEL, "Node Label Test");
  WSNPacketItem nodeIP(WSN::PACKET_TAG_NODE_IP, ip, 4);
  WSNPacketItem nodePort(WSN::PACKET_TAG_NODE_PORT, config.udp.listeningPort);

  discoverResponsePacket.setItem(0, &nodeLabel);
  discoverResponsePacket.setItem(1, &nodeIP);
  discoverResponsePacket.setItem(2, &nodePort);

  UDPServer->sendResponsePacket(&discoverResponsePacket);

  this->_addToNodeLinks(receivedPacket, WSN::isIPFromSubnet(WSNUDPServer::getInstance()->remoteIP()));

  Serial.println(discoverResponsePacket.toString());

  if(receivedFlags.nodeType == WSN_NODE_TYPE_SINK)
  {
    WSNNode::registerNode(UDPServer->remoteIP(), UDPServer->remotePort(), receivedHeader.nodeID);
  }

  WSN::logStage("END Handle request discover");
}

void WSNHeadNode::_handleRegisterRequest(WSNPacket* receivedPacket)
{
  WSN::logStage("REGISTER Request Head Node");
  WSN::logMessage("Received packet:");
  WSN::logMessage(receivedPacket->toString());

  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

  bool isFromSubnet = WSN::isIPFromSubnet(WSNUDPServer::getInstance()->remoteIP());

  if(isFromSubnet)
  {
    if(header.destinationNodeID == config.node.ID)
    {
        this->_addToNodeLinks(receivedPacket, true/*isChild*/);
    }

    //Redirect to parent so that it reaches sink.
    if(config.parent.isSet())
    {
      WSN::logStage(String("REDIRECT register request TO Parent FROM AP subnet node #") + header.nodeID);
      //TODO set parent id in packet if it is not set( == 0)

      WSNUDPServer::getInstance()->sendPacket(receivedPacket, config.parent.nodeIP, config.parent.nodePort);
    }
    else
    {
      WSN::logMessage("No prent => No redirect.");
    }
  }
  else
  {
    //Siblings not registering to a head node from same network unless it is gateway
    WSN::logMessage("Node is not from subnet (it is a sibling or a parent), so IGNORING packet.");
  }


  WSN::logStage("End Register Request Head Node");
}

void WSNHeadNode::_handleRegisterResponse(WSNPacket* receivedPacket)
{
  WSN::logStage("Register Response HEAD NODE");
  WSN::logMessage("Received packet:");
  WSN::logMessage(receivedPacket->toString());

  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

  bool isFromSubnet = WSN::isIPFromSubnet(WSNUDPServer::getInstance()->remoteIP());
  if(isFromSubnet)
  {
    WSN::logMessage("[ERROR] This packet should not come from subnet.");
    WSN::logMessage(String("FROM: ") + WSNUDPServer::getInstance()->remoteIP() + ":" + WSNUDPServer::getInstance()->remotePort());
    return;
  }

  if(header.destinationNodeID == config.node.ID)
  {
    //TODO set config if response has attributes with different values
  }
  else
  {
    //Redirect if node is in child nodes list
    WSNNodeLink* childNodeLink = WSNChildNodeLinks::get(header.destinationNodeID);
    if(childNodeLink != NULL)
    {
      WSN::logMessage(String("Redirect register for child Node #") + header.destinationNodeID);
      WSNUDPServer::getInstance()->sendPacket(receivedPacket, childNodeLink->nodeIP, childNodeLink->nodePort);
    }
    else
    {
      //Redirect to all head node childs
      WSN::logMessage(String("Node #") + header.destinationNodeID + " is not in childs list.");
      childNodeLink = WSNChildNodeLinks::getFirst();
      bool hasChildHeadNode = false;
      while(childNodeLink != NULL)
      {
        if(childNodeLink->nodeType == WSN_NODE_TYPE_HEAD_NODE)
        {
          WSN::logMessage("Redirecting to child Head Node: ");
          WSNUDPServer::getInstance()->sendPacket(receivedPacket, childNodeLink->nodeIP, childNodeLink->nodePort);
          hasChildHeadNode = true;
        }

        childNodeLink = childNodeLink->next;
      }
      if(!hasChildHeadNode)
      {
        WSN::logMessage("This node does not have any head node childs. So no rdirect.");
      }
    }
  }

  WSN::logStage("END Register Response HEAD NODE");
}

void WSNHeadNode::_handleHeartbeatRequest(WSNPacket* receivedPacket)
{
    //TODO
}

void WSNHeadNode::_handleHeartbeatResponse(WSNPacket* receivedPacket)
{
  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

  if(WSN::isIPFromSubnet(WSNUDPServer::getInstance()->remoteIP()))
  {
    WSNUDPServer::getInstance()->sendPacket(receivedPacket, config.parent.nodeIP, config.heartbeat.UDPPort);
  }
  if(flags.nodeType == WSN_NODE_TYPE_SINK)
  {
  //  WSN::logMessage(String("REDIRECT heartbeat to AP subnet from Sink #") + header.nodeID);
    WSNUDPServer::getInstance()->sendPacketSubnet(receivedPacket);
    WSN::ledSignal(header.functionID);
  }
  //else ignore
}


void WSNHeadNode::_handleQueryRequest(WSNPacket* receivedPacket)
{
  WSN::logStage("QueryRequest HEAD NODE");
  WSNUDPServer::getInstance()->sendPacketSubnet(receivedPacket);
  WSNNode::_handleQueryRequest(receivedPacket);
  //TODO

  WSN::logStage("END QueryRequest HEAD NODE");
}

void WSNHeadNode::_handleQueryResponse(WSNPacket* receivedPacket)
{
  WSN::logStage("QueryResponse HEAD NODE");

  WSNUDPServer::getInstance()->sendPacket(receivedPacket);
  WSNNode::_handleQueryResponse(receivedPacket);
  //TODO

  WSN::logStage("END QueryResponse HEAD NODE");
}

void WSNHeadNode::_handleConfigRequest(WSNPacket* receivedPacket)
{
  WSN::logStage("ConfigRequest HEAD NODE");
  WSNPacketHeader header = receivedPacket->getHeader();
  WSNPacketFlags flags(header.flags);

//if it is from parent
  if(header.destinationNodeID == 0 && header.nodeID == config.parent.nodeID)
  {
    WSNUDPServer::getInstance()->sendPacketSubnet(receivedPacket);
  }

  WSNNode::_handleConfigRequest(receivedPacket);

  WSN::logStage("END ConfigRequest HEAD NODE");
}

void WSNHeadNode::_handleConfigResponse(WSNPacket* receivedPacket)
{
  WSN::logStage("ConfigResponse HEAD NODE");
  WSNNode::_handleConfigResponse(receivedPacket);

  WSN::logStage("END ConfigResponse HEAD NODE");
}
