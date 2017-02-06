#ifndef WSN_NODE_LINKS_H
#define WSN_NODE_LINKS_H

#include "WSNNodeLink.h"

class WSNNodeLinks
{
protected:
  static WSNNodeLink* _nodeLinks; //WSNNodeLnkList.cpp
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

#endif //WSN_NODE_LINK_LIST_H
