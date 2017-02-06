#ifndef WSN_HEAD_NODE_H
#define WSN_HEAD_NODE_H

#include "WSNNode.h"

class WSNHeadNode: public WSNNode{
  protected:

    void _handleDiscoverRequest(WSNPacket* receivedPacket);
    void _handleDiscoverResponse(WSNPacket* receivedPacket);

    void _handleRegisterRequest(WSNPacket* receivedPacket);
    void _handleRegisterResponse(WSNPacket* receivedPacket);

    void _handleQueryRequest(WSNPacket* receivedPacket);
    void _handleQueryResponse(WSNPacket* receivedPacket);

    // void _addToNodeLinks(WSNPacket* receivedPacket, bool isChild);

    void _handleConfigRequest(WSNPacket* receivedPacket);
    void _handleConfigResponse(WSNPacket* receivedPacket);

    void _handleHeartbeatRequest(WSNPacket* receivedPacket);
    void _handleHeartbeatResponse(WSNPacket* receivedPacket);
  public:
    void handlePacket(WSNPacket* receivedPacket);
    bool isParentNode(uint8_t nodeType, IPAddress nodeIP);

    static void discover();
    // static void registerNode(IPAddress addressTo, uint portTo);
};

#endif //WSN_HEAD_NODE_H
