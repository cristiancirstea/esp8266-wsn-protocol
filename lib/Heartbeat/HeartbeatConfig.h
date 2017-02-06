#ifndef heartbeat_config_h
#define heartbeat_config_h
#include <functional>

typedef std::function<void(void)> heartbeatCallback_t;

#ifndef HEARTBEAT_UDP_PORT
#define HEARTBEAT_UDP_PORT 12345
#endif

#define HEARTBEAT_MODE_UDP 1
#define HEARTBEAT_MODE_HTTP 2
#define HEARTBEAT_MODE_UDP_HTTP 3

struct HeartbeatConfig{

  bool enabled = true;
  int intervalMs = 5000;
  int tickerHandlerID = -1;
  bool fireOnSetup = true;
  heartbeatCallback_t callback = NULL;
  int UDPPort = HEARTBEAT_UDP_PORT;
  bool enableLog = false;
  int mode = HEARTBEAT_MODE_UDP;
  bool sendToAP = true;
};
#endif
