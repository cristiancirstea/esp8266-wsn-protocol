#ifndef wsn_heartbeat_h
#define wsn_heartbeat_h

#include "IPAddress.h"
#include "WSNClient.h"
#include "WSNConfig.h"
#include "WSNUDPServer.h"
#include "TickerScheduler.h"
#include "WSNPacket.h"
#include "WSNSensor.h"

void heartbeat_send();

class Heartbeat{
  protected:
    //TODO make  multitone
    static Heartbeat* _instance;
    static bool _instanceCreated;

    TickerScheduler _tickerScheduler;
    void _sendUDP(char UDPHeartbeatBuffer[], bool redirected);
    void _sendHTTP();
    void _sendHTTP(int nodeID);
    void _sendPacket(WSNPacket* packet);
    Heartbeat();
  public:

    void begin();
    void sendMessage(char UDPHeartbeatBuffer[], bool redirected);
    void update();
    void send();
    void end();

    static Heartbeat* getInstance()
    {
      if(!_instanceCreated)
      {
        _instance = new Heartbeat();
        _instanceCreated = true;
      }
      return _instance;
    }
};

#endif
