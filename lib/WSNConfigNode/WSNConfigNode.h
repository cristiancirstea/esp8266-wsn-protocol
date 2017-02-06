#ifndef wsn_config_node_h
#define wsn_config_node_h
#ifndef NODE_ID
#define NODE_ID 1
#endif
#ifndef WSN_NODE_TYPE_NODE
#define WSN_NODE_TYPE_NODE 0
#endif
#ifndef WSN_NODE_TYPE_HEAD_NODE
#define WSN_NODE_TYPE_HEAD_NODE 1
#endif
#ifndef WSN_NODE_TYPE_SINK
#define WSN_NODE_TYPE_SINK 3
#endif
#ifndef NODE_TYPE
#define NODE_TYPE 0 /* node */
#endif
#ifndef NODE_LABEL
#define NODE_LABEL "Node"
#endif
#include <Arduino.h>

struct WSNConfigNode{
private:
  int _type = NODE_TYPE;
public:
  int ID = NODE_ID;
  char label [50] = NODE_LABEL;
  bool isHeadNode()
  {
    return this->getType() == WSN_NODE_TYPE_HEAD_NODE;
  }

  int getType()
  {
    return this->_type;
  }

  String getTypeToString()
  {
    switch (this->_type) {
      case WSN_NODE_TYPE_HEAD_NODE:
        return "HeadNode";
      case WSN_NODE_TYPE_NODE:
        return "Node";
      case WSN_NODE_TYPE_SINK:
        return "Sink";
    }
    return "unknown";
  }

  bool setType(int type)
  {
    if(
      type == WSN_NODE_TYPE_HEAD_NODE
      || type == WSN_NODE_TYPE_NODE
      || type == WSN_NODE_TYPE_SINK
    )
    {
      this->_type = type;
      return true;
    }
    return false;
  }
};
#endif
