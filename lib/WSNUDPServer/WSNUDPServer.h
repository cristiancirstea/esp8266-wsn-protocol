#ifndef WSN_UDP_SERVER_H
#define WSN_UDP_SERVER_H

#include <Arduino.h>

#include <WiFiUdp.h>
#include "WSNConfig.h"
#include "WSNPacket.h"
#include "WSNNodeLinks.h"
#include "WSNNode.h"
#include "WSNHeadNode.h"

class WSNUDPServer: public WiFiUDP{

  protected:
    static WSNUDPServer* _instance;

  public:
    //TODO make this functions static
    void handleMessage();
    // static void send_message()
    void sendPacket(WSNPacket* packet, IPAddress ip, unsigned port);
    void sendPacket(WSNPacket* packet);
    void sendPacket(WSNPacket* packet, unsigned port);
    void sendPacketSubnet(WSNPacket* packet);
    void sendPacketSubnet(WSNPacket* packet, unsigned port);
    void sendResponsePacket(WSNPacket* packet);

    static WSNUDPServer* getInstance();
};

#endif //WSN_UDP_SERVER_H
