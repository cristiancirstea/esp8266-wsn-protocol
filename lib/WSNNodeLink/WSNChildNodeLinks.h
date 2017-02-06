#ifndef WSN_CHILD_NODE_LINKS_H
#define WSN_CHILD_NODE_LINKS_H

#include "WSNNodeLinks.h"

class WSNChildNodeLinks : public WSNNodeLinks{
protected:
  static WSNNodeLink* _nodeLinks; //WSNChildNodeLnkList.cpp
public:
  static WSNNodeLink* getFirst();
  static WSNNodeLink* add(WSNNodeLink* nodeLink);
  static WSNNodeLink* add(WSNNodeLink* nodeLink, bool override);
  static bool remove(WSNNodeLink* nodeLink);
  static bool remove(int nodeID);
  static WSNNodeLink* get(unsigned nodeID);
  static void clear();
  static bool isEmpty();
};

#endif //WSN_CHILD_NODE_LINKS_H
