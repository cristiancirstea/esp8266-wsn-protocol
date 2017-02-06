#include "WSNPacket.h"

WSNPacket::WSNPacket(unsigned itemCount)
{
  if(WSN_PACKET_SERIAL_DEBUG)
  {
    Serial.printf("Packet Constructor ([unsigned] %u)\n", itemCount);
  }
  if(itemCount > 0)
  {
    this->_items = new WSNPacketItem[itemCount];
    this->_itemCount = itemCount;
  }
  else
  {
    this->_items = NULL;
    this->_itemCount = 0;
  }

  this->_header.attributeCount = this->_itemCount;
}

//IT DOESN'T COPY DATA GIVEN, IT USES IT
WSNPacket::WSNPacket(unsigned itemCount, WSNPacketItem* pItems)
{
  if(WSN_PACKET_SERIAL_DEBUG)
  {
    Serial.printf("Packet Constructor2 ([unsigned] %u, [WSNPacketItem*] %p)\n", itemCount, pItems);
  }
  if(itemCount > 0)
  {
    if(pItems != NULL)
    {
        this->_items = pItems;
    }
    else
    {
      this->_items = new WSNPacketItem[itemCount];
    }
    this->_itemCount = itemCount;
  }
  else
  {
    this->_items = NULL;
    this->_itemCount = 0;
  }
  this->_header.attributeCount = this->_itemCount;
}

WSNPacket::~WSNPacket()
{
  if(WSN_PACKET_SERIAL_DEBUG)
  {
    Serial.printf("Packet Destructor!!!\n");
  }

  if(this->_itemCount > 0)
  {
    delete[] this->_items;
    this->_items = NULL;
    this->_itemCount = 0;
    this->_header.attributeCount = 0;
  }
}

WSNPacketHeader WSNPacket::getHeader()
{
  return this->_header;
}


bool WSNPacket::setHeader(WSNPacketHeader header)
{
  if(this->_validateHeader(header))
  {
    this->_header = header;

    this->_header.attributeCount = this->_itemCount;
    return true;
  }

  return false;
}

bool WSNPacket::setHeader(uint16_t nodeID, uint16_t functionID, uint16_t destNodeID, uint16_t flags)
{
  WSNPacketHeader header;

  header.nodeID = nodeID;
  header.functionID = functionID;
  header.destinationNodeID = destNodeID;
  header.flags = flags;

  this->setHeader(header);
}

bool WSNPacket::_validateHeader(WSNPacketHeader header)
{
  //TODO validate header !!!
  return true;
}

bool WSNPacket::validate()
{
  if(!this->_validateHeader(this->_header))
  {
    //TODO throw or log an error
    return false;
  }

  //TODO validate data

  return true;
}

const char* WSNPacket::serialize()
{
  char* pBuffer;
  // if(WSN_PACKET_SERIAL_DEBUG)
  // {
  //   Serial.printf(
  //     "\n------\n header size >> %d\n data h size >> %d \n",
  //     PACKET_HEADER_SIZE_BYTES,
  //     this->_header.packetDataSizeBytes
  //   );
  //   //Serial.println(this->toString());
  // }
  int bufferSizeBytes = WSN::PACKET_HEADER_SIZE_BYTES + this->_header.packetDataSizeBytes;

  pBuffer = new char[bufferSizeBytes + 1];
  char* pBufferCursor = pBuffer;

  memcpy(pBufferCursor, &this->_header.version, 2);
  pBufferCursor = pBufferCursor + 2;

  memcpy(pBufferCursor, &this->_header.functionID, 2);
  pBufferCursor = pBufferCursor + 2;

  memcpy(pBufferCursor, &this->_header.nodeID, 2);
  pBufferCursor = pBufferCursor + 2;

  memcpy(pBufferCursor, &this->_header.destinationNodeID, 2);
  pBufferCursor = pBufferCursor + 2;

  memcpy(pBufferCursor, &this->_header.transactionID, 2);
  pBufferCursor = pBufferCursor + 2;

  memcpy(pBufferCursor, &this->_header.flags, 2);
  pBufferCursor = pBufferCursor + 2;

  memcpy(pBufferCursor, &this->_header.attributeCount, 2);
  pBufferCursor = pBufferCursor + 2;

  memcpy(pBufferCursor, &this->_header.packetDataSizeBytes, 2);
  pBufferCursor = pBufferCursor + 2;

  for(int i = 0; i < this->_itemCount; i++)
  {
    if(WSN_PACKET_SERIAL_DEBUG)
    {
      Serial.println("--------------- Serializing ---------");
    }
    memcpy(pBufferCursor, this->getItem(i)->serialize(), this->getItem(i)->getTotalLengthBytes());
    pBufferCursor = pBufferCursor + this->getItem(i)->getTotalLengthBytes();
    if(WSN_PACKET_SERIAL_DEBUG)
    {
      Serial.println("--------------- Item serialized---------");
      Serial.println(this->getItem(i)->toString());
    }
  }

  //TODO check if total length of items = header.packetDataSizeBytes

  if(WSN_PACKET_SERIAL_DEBUG)
  {
    // Serial.printf("Packet Serialized >> %s\n", pBuffer);
    Serial.printf("Buffer Size Bytes >> %d\n", bufferSizeBytes);
    Serial.println("Raw data >>");
    for(int i=0; i<bufferSizeBytes; i++)
    {
      Serial.printf("%d ", pBuffer[i]);
    }
    Serial.printf("\n");
  }
  return pBuffer;
}

WSNPacket* WSNPacket::deserialize(char * dataBuffer, unsigned sizeBytes)
{
  //TODO make a list of packet items or get somehow number of items/attributes
    if(WSN_PACKET_SERIAL_DEBUG)
    {
      Serial.println("-------- PACKET DESERIALIZE--------------");
      Serial.println("Raw data >>");
      for(int i=0; i<sizeBytes; i++)
      {
        Serial.printf("%d ", dataBuffer[i]);
      }
      Serial.printf("\n");
    }
    unsigned offsetBytes = 0;
    WSNPacketHeader header;

    memcpy(&header.version, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    memcpy(&header.functionID, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    memcpy(&header.nodeID, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    memcpy(&header.destinationNodeID, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    memcpy(&header.transactionID, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    memcpy(&header.flags, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    memcpy(&header.attributeCount, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    memcpy(&header.packetDataSizeBytes, dataBuffer + offsetBytes, 2);
    offsetBytes += 2;

    if(sizeBytes != header.packetDataSizeBytes + WSN::PACKET_HEADER_SIZE_BYTES)
    {
      Serial.printf(
        "!!!!!!!!!!!!!!!!!! PACKET SIZE (%d) != HEADER_SIZE + HEADER_DATA_SIZE (%d + %d)!!!!!!!!!!!!!!",
        sizeBytes,
        header.packetDataSizeBytes,
        WSN::PACKET_HEADER_SIZE_BYTES
      );
      return NULL;
    }

    WSNPacket* decodedPacket = new WSNPacket(header.attributeCount); //TODO

    //TODO
    //TODO
    //TODO
    //TODO Fix this adding to packet data size bug !!!!!!!!!!!
    uint16_t temp = header.packetDataSizeBytes;
    header.packetDataSizeBytes = 0;
    decodedPacket->setHeader(header);
    header.packetDataSizeBytes = temp;

    if(header.attributeCount > 0)
    {
      unsigned itemCount = 0;
      unsigned iterationCount = 0;

      // if(WSN_PACKET_SERIAL_DEBUG)
      // {
      //   Serial.println("--------STARTIN ATTRIBUTE DES --------------");
      // }
      while(
        offsetBytes < header.packetDataSizeBytes + WSN::PACKET_HEADER_SIZE_BYTES
        && iterationCount < WSN_PACKET_MAX_ATTRIBUTE_COUNT
      )
      {
        iterationCount++;
        WSNPacketItem* packetItem = WSNPacketItem::deserialize(dataBuffer + offsetBytes, sizeBytes - offsetBytes);
        if(packetItem != NULL)
        {
          offsetBytes += packetItem->getTotalLengthBytes();
          if(WSN_PACKET_SERIAL_DEBUG)
          {
            Serial.println("~Deserialized Packet Item >>" + packetItem->toString());
          }
          decodedPacket->setItem(itemCount, packetItem);



          itemCount++;
        }
        else
        {
          //TODO ?????????????????
          Serial.printf("!!!!!!!!!!!! PACKET %d ITEM DESERIALIED NULL !!!!!!!!!!", iterationCount);
          break;
        }
        delete packetItem;
      }
      // if(WSN_PACKET_SERIAL_DEBUG)
      // {
      //   Serial.println("--------END ATTRIBUTE DES --------------");
      // }

    }

    if(WSN_PACKET_SERIAL_DEBUG)
    {
      Serial.println("Packet deserialized >>" + decodedPacket->toString());
      Serial.println("--------END PACKET DESERIALIZE--------------");
    }
    return decodedPacket;
}

bool WSNPacket::setItem(unsigned id, WSNPacketItem* item)
{
  if(id >= this->_itemCount)
  {
    return false;
  }
  if(item == NULL)
  {
    return false;
  }

  this->_items[id].setTag(item->getTag());
  this->_items[id].setValue((uint8_t*)item->getValue(), item->getLength());
//TODO what happens when same item is set multiple times? bad things, dataSize is incremented
  this->_header.packetDataSizeBytes += item->getTotalLengthBytes();


  if(WSN_PACKET_SERIAL_DEBUG)
  {
    Serial.println("-------------- ADD PACKET --------");
    Serial.println(
      "Added packet item >> " + item->toString() +
      "\nSize Bytes  packet item>>" + this->_header.packetDataSizeBytes
    );
    Serial.println("-------------- END ADD PACKET --------");
  }

  return true;
}


WSNPacketItem* WSNPacket::getItemByTag(uint32_t tagID)
{
  WSNPacketItem* theItem = NULL;

  for(int i = 0; i < this->_itemCount; i++)
  {
    theItem = this->getItem(i);
    if(theItem != NULL)
    {
      if(theItem->getTag() == tagID)
      {
        break;
      }

      theItem = NULL;
    }
  }

  return theItem;
}

WSNPacketItem* WSNPacket::getItem(unsigned id)
{
  if(id >= this->_itemCount)
  {
    return NULL;
  }

  return &this->_items[id];
}

unsigned WSNPacket::getItemCount()
{
  return this->_itemCount;
}

uint32_t WSNPacket::getTotalLengthBytes()
{
  return this->_header.packetDataSizeBytes + WSN::PACKET_HEADER_SIZE_BYTES;
}

String WSNPacket::toString()
{
  String result = "";
  if(WSN_PACKET_SERIAL_DEBUG)
  {
    result += "------- WSNPacket -----------\n";
  }

  result  += "[";
  result += String("v ") + this->_header.version + "|";
  result += String("f ") + this->_header.functionID + "|";
  result += String("n ") + this->_header.nodeID + "|";
  result += String("dn ") + this->_header.destinationNodeID + "|";
  result += String("tr ") + this->_header.transactionID + "|";
  result += String("fl ") + this->_header.flags + "|";
  result += String("a ") + this->_header.attributeCount + "|";
  result += String("p ") + this->_header.packetDataSizeBytes + "]";

  result += String("\nFunction: ") + WSNPacket::functionIDToString(this->_header.functionID);

  WSNPacketFlags flags(this->_header.flags);
  result += String("\nFlags:\n") + flags.toString() + "\n";
  result += "Attributes:\n";
  if(this->_itemCount == 0)
  {
    result += "-";
  }

  for(int i = 0; i < this->_itemCount; i++)
  {
    WSNPacketItem* item = this->getItem(i);
    if(item != NULL)
    {
      result += item->toString() + "\n";
    }
  }

  return result;
}


String WSNPacket::functionIDToString(uint16_t functionID)
{
  switch (functionID)
  {
    case WSN::PACKET_FUNCTION_DISCOVER:
      return "Discover";

    case WSN::PACKET_FUNCTION_HEARTBEAT:
      return "Heartbeat";

    case WSN::PACKET_FUNCTION_BROADCAST_MESSAGE:
      return "Broadcast Message";

    case WSN::PACKET_FUNCTION_REGISTER:
      return "Register";

    case WSN::PACKET_FUNCTION_CONFIG:
      return "Config";

    case WSN::PACKET_FUNCTION_QUERY:
      return "Query";

    default:
      return "Unknown";
  }
}
