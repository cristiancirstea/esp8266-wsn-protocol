#include "WSNChildNodeLinks.h"


WSNNodeLink* WSNChildNodeLinks::_nodeLinks = NULL;

WSNNodeLink* WSNChildNodeLinks::getFirst()
{
  return WSNChildNodeLinks::_nodeLinks;
}

WSNNodeLink* WSNChildNodeLinks::add(WSNNodeLink* nodeLink)
{
  WSNChildNodeLinks::add(nodeLink, false);
}

WSNNodeLink* WSNChildNodeLinks::add(WSNNodeLink* nodeLink, bool override)
{
  if(nodeLink == NULL)
  {
    return NULL;
  }
  // WSNChildNodeLinks::logStage("node_link_add");
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

  // WSNChildNodeLinks::logStage("END node_link_add");
  return nodeLinkIterator;
}

WSNNodeLink* WSNChildNodeLinks::get(unsigned nodeID)
{
  // WSNChildNodeLinks::logStage(String("node_link_get(unsigned ") + nodeID +")");
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

bool WSNChildNodeLinks::remove(WSNNodeLink* nodeLinkRemoving)
{
  if(nodeLinkRemoving == NULL)
    return false;

  return WSNChildNodeLinks::remove(nodeLinkRemoving->nodeID);
}

bool WSNChildNodeLinks::remove(int nodeID)
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

void WSNChildNodeLinks::clear()
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

bool WSNChildNodeLinks::isEmpty()
{
  return (_nodeLinks == NULL);
}
