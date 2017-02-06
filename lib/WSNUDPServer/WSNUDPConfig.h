#ifndef WSN_UDP_CONFIG_H
#define WSN_UDP_CONFIG_H

struct WSNUDPConfig{
  int sendingPort = 12345;
  int listeningPort = 12345;
  int packetSize = 255;
  char defaultReplyBuffer[8] = "Pong #1";
};

#endif //WSN_UDP_CONFIG_H
