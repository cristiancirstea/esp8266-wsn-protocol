#ifndef WSN_NODE_H
#define WSN_NODE_H

#include <Arduino.h>
#include "WSNPacket.h"
#include "WSNNodeLinks.h"
#include "WSNChildNodeLinks.h"

//TEST
#include "WSNClient.h"

#include "IPAddress.h"

//TODO make this independent of config and UDPServer
class WSNNode{
  protected:

    void _handleDiscoverRequest(WSNPacket* receivedPacket);
    void _handleDiscoverResponse(WSNPacket* receivedPacket);

    void _handleRegisterRequest(WSNPacket* receivedPacket);
    void _handleRegisterResponse(WSNPacket* receivedPacket);

    void _handleQueryRequest(WSNPacket* receivedPacket);
    void _handleQueryResponse(WSNPacket* receivedPacket);
    bool _checkQueryItemValue(WSNPacketItem* packetItem);

    void _handleConfigRequest(WSNPacket* receivedPacket);
    void _handleConfigResponse(WSNPacket* receivedPacket);

    void _handleHeartbeatRequest(WSNPacket* receivedPacket);
    void _handleHeartbeatResponse(WSNPacket* receivedPacket);

    void _handleUnknownFunction(WSNPacket* receivedPacket);

    void _addToNodeLinks(WSNPacket* receivedPacket, bool isChild);
  public:
    void handlePacket(WSNPacket* receivedPacket);
    bool isParentNode(uint8_t nodeType, IPAddress nodeIP);
    void logAllNodeLinks();

    static void discover();
    static void registerNode(IPAddress addressTo, uint portTo, uint detinationNodeID);
    static bool registerToParent();
};

#endif //WSN_NODE_H
