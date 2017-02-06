#ifndef WSN_NETWORK_CONFIG_H
#define WSN_NETWORK_CONFIG_H

#define WSN_NETWORK_TYPE_STAR 1
#define WSN_NETWORK_TYPE_TREE 2
#define WSN_NETWORK_TYPE_MESH 3

#ifndef WSN_NETWORK_TYPE
#define WSN_NETWORK_TYPE WSN_NETWORK_TYPE_STAR
#endif

#include <Arduino.h>

struct WSNNetworkConfig{
  uint8_t type = WSN_NETWORK_TYPE;

  String typeToString(){
    switch (type) {
      case WSN_NETWORK_TYPE_STAR:
        return "Star";
      case WSN_NETWORK_TYPE_TREE:
        return "Tree";
      case WSN_NETWORK_TYPE_MESH:
        return "Mesh";
      default:
        return "Unknown";
    }
    return "Unknown";
  }
};

#endif //WSN_NETWORK_CONFIG_H
