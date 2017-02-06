#include "WSNNodeLinks.h"

WSNNodeLink* WSNNodeLinks::_nodeLinks = NULL;

WSNNodeLink* WSNNodeLinks::getFirst()
{
  return WSNNodeLinks::_nodeLinks;
}

WSNNodeLink* WSNNodeLinks::add(WSNNodeLink* nodeLink)
{
  WSNNodeLinks::add(nodeLink, false);
}

WSNNodeLink* WSNNodeLinks::add(WSNNodeLink* nodeLink, bool override)
{
  if(nodeLink == NULL)
  {
    return NULL;
  }
  // WSNNodeLinks::logStage("node_link_add");
  WSNNodeLink* nodeLinkIterator = _nodeLinks;
  WSNNodeLink* nodeLinkPrevious = NULL;
  while(nodeLinkIterator != NULL)
  {
    if(nodeLinkIterator->nodeID == nodeLink->nodeID)
    {
      break;
    }

    nodeLinkPrevious = nodeLinkIterator;
    nodeLinkIterator = nodeLinkIterator->next;
  }

  if(
    nodeLinkIterator == NULL
    || (nodeLinkIterator != NULL && override)
  )
  {
    if(nodeLinkIterator == NULL)
    {
      nodeLinkIterator = new WSNNodeLink;
      nodeLinkIterator->next = NULL;
    }

    if(nodeLinkPrevious != NULL)
    {
      nodeLinkPrevious->next = nodeLinkIterator;
    }

    nodeLinkIterator->nodeID = nodeLink->nodeID;
    nodeLinkIterator->nodeLabel = nodeLink->nodeLabel;
    nodeLinkIterator->nodeType = nodeLink->nodeType;
    nodeLinkIterator->nodeIP = nodeLink->nodeIP;
    nodeLinkIterator->nodePort = nodeLink->nodePort;
  }

  if(_nodeLinks == NULL)
  {
    _nodeLinks = nodeLinkIterator;
  }

  // WSNNodeLinks::logStage("END node_link_add");
  return nodeLinkIterator;
}

WSNNodeLink* WSNNodeLinks::get(unsigned nodeID)
{
  // WSNNodeLinks::logStage(String("node_link_get(unsigned ") + nodeID +")");
  WSNNodeLink* nodeLinkIterator = _nodeLinks;
  while(nodeLinkIterator != NULL)
  {
    if(nodeLinkIterator->nodeID == nodeID)
    {
      break;
    }

    nodeLinkIterator = nodeLinkIterator->next;
  }

  return nodeLinkIterator;
}

bool WSNNodeLinks::remove(WSNNodeLink* nodeLinkRemoving)
{
  if(nodeLinkRemoving == NULL)
    return false;

  return WSNNodeLinks::remove(nodeLinkRemoving->nodeID);
}

bool WSNNodeLinks::remove(int nodeID)
{
  WSNNodeLink* nodeLinkIterator = _nodeLinks;
  WSNNodeLink* nodeLinkPrevious = NULL;

  while(nodeLinkIterator != NULL)
  {
    if(nodeLinkIterator->nodeID == nodeID)
    {
      if(nodeLinkPrevious != NULL)
      {
        nodeLinkPrevious->next = nodeLinkIterator->next;
        delete nodeLinkIterator;
        nodeLinkIterator = NULL;
        return true;
      }
    }

    nodeLinkPrevious = nodeLinkIterator;
    if(nodeLinkIterator != NULL)
    {
      nodeLinkIterator = nodeLinkIterator->next;
    }
  }

  return false;
}

void WSNNodeLinks::clear()
{
  WSNNodeLink* nodeLinkIterator = _nodeLinks;
  WSNNodeLink* nodeToDelete = NULL;
  while(nodeLinkIterator != NULL)
  {
    nodeToDelete = nodeLinkIterator;
    nodeLinkIterator = nodeLinkIterator->next;
    delete nodeToDelete;
  }

  _nodeLinks = NULL;
}

bool WSNNodeLinks::isEmpty()
{
  return (_nodeLinks == NULL);
}
