#include "WSNPacketItem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

WSNPacketItem::WSNPacketItem()
{
  this->_value = NULL;
  this->_length = 0;
}

WSNPacketItem::WSNPacketItem(uint32_t tag, const char* value)
{
  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf("Packet item constructor1 (%u, %s)\n", tag, value);
  }
  this->_value = NULL;
  this->setTag(tag);
  this->setValue(value);
}

WSNPacketItem::WSNPacketItem(uint32_t tag, uint8_t* value, uint16_t size)
{
  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf("Packet item constructor2 (%u, %p*, %d)\n", tag, value, size);
  }
  this->_value = NULL;
  this->setTag(tag);
  this->setValue(value, size);
}

WSNPacketItem::WSNPacketItem(WSNPacketItem& packetItem)
{
  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.println("Packet item copy constructor (Packet item >> " + packetItem.toString() + ")");
  }
  this->_value = NULL;
  this->setTag(packetItem.getTag());
  this->setValue(packetItem.getValue());
}

WSNPacketItem::WSNPacketItem(uint32_t tag, int32_t value)
{
  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf("Packet item construsctor3 (%u, %u)\n", tag, value);
  }
  this->_value = NULL;
  this->setTag(tag);
  this->setValue(value);
}

WSNPacketItem::WSNPacketItem(uint32_t tag, float value)
{
  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf("Packet item construsctor4 (%u, %d)\n", tag, value);
  }
  this->_value = NULL;
  this->setTag(tag);
  this->setValue(value);
}

WSNPacketItem::~WSNPacketItem()
{
  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf("Packet item destructor!!!\n");
  }
  if(this->_value != NULL)
  {
    delete this->_value;
    this->_value = NULL;
  }
}

void WSNPacketItem::_setLength(int length)
{
  this->_length = length;
}

uint32_t WSNPacketItem::getLength()
{
  return this->_length;
}

uint32_t WSNPacketItem::getTag()
{
  return this->_tag;
}

bool WSNPacketItem::_validateTag(uint32_t tag)
{
  for(int i = 0; i < WSN::PACKET_ITEM_ALLOWED_TAGS_LENGTH; i++)
  {
    if(tag == this->allowedTags[i])
    {
      return true;
    }
  }

  return false;
}

bool WSNPacketItem::setTag(uint32_t tag)
{
  if(!this->_validateTag(tag))
  {
    Serial.printf("Invalid tag on Packet Item >> %d\n", tag);
    return false;
  }

  this->_tag = tag;
  return true;
}

char* WSNPacketItem::getValue()
{
  return this->_value;
}

uint32_t WSNPacketItem::getValueUInt32()
{
  return *((uint32_t*)this->getValue());
}

uint16_t WSNPacketItem::getValueUInt16()
{
  return *((uint16_t*)this->getValue());
}

uint8_t WSNPacketItem::getValueUInt8()
{
  return *((uint8_t*)this->getValue());
}


bool  WSNPacketItem::setValue(const char* value)
{
  if(value == NULL)
  {
    this->setValueNull();
    return true;
  }
  if(this->_value != NULL)
  {
    delete this->_value;
    this->_setLength(0);
  }
  unsigned valueSize = strlen(value) + 1;

  this->_value = new char[valueSize];
  if(this->_value == NULL)
  {
    return false;
  }
  for(int i = 0; i < valueSize; i++)
  {
    this->_value[i] = value[i];
  }

  this->_setLength(valueSize);

  return true;
}

bool  WSNPacketItem::setValue(const uint8_t* value, uint16_t length)
{
  if(value == NULL)
  {
    this->setValueNull();
    return true;
  }
  if(this->_value)
  {
    delete this->_value;
    this->_setLength(0);
  }

  this->_value = new  char[length];
  if(this->_value == NULL)
  {
    return false;
  }
  memcpy(this->_value, value, length);

  this->_setLength(length);

  return true;
}

bool WSNPacketItem::setValue(int32_t value)
{
  return this->setValue((uint8_t*)&value, sizeof(int32_t));
}

bool WSNPacketItem::setValue(float value)
{
  return this->setValue((uint8_t*)&value, sizeof(float));
}

void WSNPacketItem::setValueNull()
{
  if(this->_value != NULL)
  {
    delete this->_value;
  }
  this->_value = NULL;
  this->_setLength(0);
}

bool WSNPacketItem::isValueNull()
{
  return (this->_value == NULL && this->_length == 0);
}

uint32_t WSNPacketItem::getTotalLengthBytes()
{
  return WSN::PACKET_ITEM_TAG_SIZE_BYTES + WSN::PACKET_ITEM_LENGTH_SIZE_BYTES + this->getLength();
}

const char* WSNPacketItem::serialize()
{
  /*
  * TLV
  * tag - 4 bytes
  * length - 4 bytes
  * Value - N bytes; N=length of value
  */
  char* pBuffer;

  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    if(this->isValueNull())
    {
      Serial.println("Item raw value >> NULL");
    }
    else
    {
      Serial.printf("Item raw value >> %s\n", this->getValue());
    }
  }

  int bufferSize = WSN::PACKET_ITEM_TAG_SIZE_BYTES +
    WSN::PACKET_ITEM_LENGTH_SIZE_BYTES +
    this->getLength();

  pBuffer = new char[bufferSize];
  char* pBufferCursor = pBuffer;

  memcpy(pBufferCursor, &this->_tag, WSN::PACKET_ITEM_TAG_SIZE_BYTES);
  pBufferCursor = pBufferCursor + WSN::PACKET_ITEM_TAG_SIZE_BYTES;

  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf(
      "buffer1 >> %d %d %d %d\n",
      pBuffer[0],
      pBuffer[1],
      pBuffer[2],
      pBuffer[3]
    );
  }

  memcpy(pBufferCursor, &this->_length, WSN::PACKET_ITEM_LENGTH_SIZE_BYTES);
  pBufferCursor = pBufferCursor + WSN::PACKET_ITEM_LENGTH_SIZE_BYTES;

  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf(
      "buffer2 >> %d %d %d %d\n",
      pBuffer[4],
      pBuffer[5],
      pBuffer[6],
      pBuffer[7]
    );
  }

  if(!this->isValueNull())
  {
    memcpy(pBufferCursor, this->_value, this->_length * sizeof(char));
    if(WSN_PACKET_ITEM_SERIAL_DEBUG)
    {
      Serial.printf(
        "buffer3 [4 bytes]>> %d %d %d %d\n",
        pBuffer[8],
        pBuffer[9],
        pBuffer[10],
        pBuffer[11]
      );
    }
    pBufferCursor = pBufferCursor + this->_length;
  }

  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.printf(
      "Value length >> %d\nBuffer length >> %d\n",
      this->_length,
      this->getTotalLengthBytes()
    );
    Serial.println("To String >> " + this->toString());
  }

  return pBuffer;
}

WSNPacketItem* WSNPacketItem::deserialize(char* itemData, unsigned itemDataSize)
{
  //TODO return null if somethig goes wrong!!!!!
  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.println("-------- PACKET ITEM DESERIALIZE--------------");
    Serial.println("Raw data >>");
    for(int i=0; i<itemDataSize; i++)
    {
      Serial.printf("%d ", itemData[i]);
    }
    Serial.printf("\nItem Data Size >> %d\n", itemDataSize);
  }
  WSNPacketItem* packetItem = new WSNPacketItem();
  uint32_t tag = 0, length = 0;
  unsigned offsetBytes = 0;

  memcpy(&tag, itemData, WSN::PACKET_ITEM_TAG_SIZE_BYTES);
  offsetBytes += WSN::PACKET_ITEM_TAG_SIZE_BYTES;

  memcpy(&length, itemData + offsetBytes, WSN::PACKET_ITEM_LENGTH_SIZE_BYTES);
  offsetBytes += WSN::PACKET_ITEM_LENGTH_SIZE_BYTES;

  if(length > 0)
  {
    char dataBuffer[length];
    memcpy(dataBuffer, itemData + offsetBytes, length * sizeof(char));

    packetItem->setValue((uint8_t*)dataBuffer, length);
  }
  else
  {
    packetItem->setValueNull();
  }

  packetItem->setTag(tag);

  if(WSN_PACKET_ITEM_SERIAL_DEBUG)
  {
    Serial.println("Packet data >>" + packetItem->toString());
    Serial.println("-------- END PACKET ITEM DESERIALIZE--------------");
  }
  return packetItem;
}

const String WSNPacketItem::toString()
{
  return String("[") +
    this->getTag() + "|" +
    this->getLength() + "|" +
    this->valueToString() + "] - " + WSNPacketItem::tagToString(this->getTag());
}

const String WSNPacketItem::valueToString()
{
  if(this->isValueNull())
  {
    return String("NULL");
  }
  switch (this->getTag())
  {
    case WSN::PACKET_TAG_NODE_ID:
    case WSN::PACKET_TAG_PARENT_NODE_ID:
    case WSN::PACKET_TAG_NODE_PORT:
    case WSN::PACKET_TAG_SENSOR_ID:
    case WSN::PACKET_TAG_SENSOR_VALUE: //TODO
    case WSN::PACKET_TAG_ERROR_ID:
    case WSN::PACKET_TAG_BATTERY_LEVEL: //TODO
    case WSN::PACKET_TAG_SCAN_WIFI_DISTANCE: //TODO
    case WSN::PACKET_TAG_SENSOR_TYPE:
      return String(
        (uint8_t)this->_value[0]) + " " +
        (uint8_t)this->_value[1] + " " +
        (uint8_t)this->_value[2] + " " +
        (uint8_t)this->_value[3];
    break;
    case WSN::PACKET_TAG_NODE_IP:
      return String(
        (uint8_t)this->_value[0]) + "." +
        (uint8_t)this->_value[1] + "." +
        (uint8_t)this->_value[2] + "." +
        (uint8_t)this->_value[3];
    break;
    case WSN::PACKET_TAG_NODE_TYPE:
      return String(
        (uint8_t)this->_value[0]) + " " +
        (uint8_t)this->_value[1];
    break;
    case WSN::PACKET_TAG_NODE_WIFI_MODE:
      switch ((uint8_t)this->_value[0])
      {
        case 0:
          return "WIFI_OFF";
        case 1:
          return "WIFI_STA";
        case 2:
          return "WIFI_AP";
        case 3:
          return "WIFI_AP_STA";
        default:
          return String("Unknown(") + (uint8_t)this->_value[0] + ")";
      }
      break;
    case WSN::PACKET_TAG_NETWORK_TYPE:
      switch ((uint8_t)this->_value[0])
      {
        case 1:
          return "Star";
        case 2:
          return "Tree";
        case 3:
          return "Mesh";
        default:
          return String("Unknown(") + (uint8_t)this->_value[0] + ")";
      }
      break;
    case WSN::PACKET_TAG_NODE_LABEL:
    case WSN::PACKET_TAG_NODE_AP_SSID:
    case WSN::PACKET_TAG_NODE_AP_PASSWORD:
    case WSN::PACKET_TAG_SENSOR_LABEL:
    case WSN::PACKET_TAG_ERROR_MESSAGE:
    case WSN::PACKET_TAG_CUSTOM_MESSAGE:
    case WSN::PACKET_TAG_SCAN_WIFI_SSID:
      return String(this->_value);
    break;
    default:
      return String("\nUnhandled tag value") + this->getTag() + "\n";
  }

  return "";
}

String WSNPacketItem::tagToString(uint16_t tagID)
{
  switch (tagID)
  {
    case WSN::PACKET_TAG_NODE_ID:
      return "Node ID";
    case WSN::PACKET_TAG_NODE_LABEL:
      return "Node Label";
    case WSN::PACKET_TAG_NODE_IP:
      return "Node IP";
    case WSN::PACKET_TAG_NODE_PORT:
      return "Node Port";
    case WSN::PACKET_TAG_NODE_TYPE:
      return "Node Type";
    case WSN::PACKET_TAG_NODE_WIFI_MODE:
      return "Node WiFi Mode";
    case WSN::PACKET_TAG_NODE_AP_SSID:
      return "Node AP SSID";
    case WSN::PACKET_TAG_NODE_AP_PASSWORD:
      return "Node AP Password";
    case WSN::PACKET_TAG_SENSOR_ID:
      return "Sensor ID";
    case WSN::PACKET_TAG_SENSOR_LABEL:
      return "Sensor Label";
    case WSN::PACKET_TAG_SENSOR_VALUE:
      return "Sensor Value";
    case WSN::PACKET_TAG_SENSOR_TYPE:
      return "Sensor Type";
    case WSN::PACKET_TAG_ERROR_ID:
      return "Error ID";
    case WSN::PACKET_TAG_ERROR_MESSAGE:
      return "Error Message";
    case WSN::PACKET_TAG_BATTERY_LEVEL:
      return "Battery Level";
    case WSN::PACKET_TAG_CUSTOM_MESSAGE:
      return "Custom Message";
    case WSN::PACKET_TAG_NETWORK_TYPE:
      return "Network Type";
    case WSN::PACKET_TAG_PARENT_NODE_ID:
      return "Parent Node ID";
    case WSN::PACKET_TAG_SCAN_WIFI_SSID:
      return "Scan WiFi SSID";
    case WSN::PACKET_TAG_SCAN_WIFI_DISTANCE:
      return "Scan WiFi Distance";
    default:
      return String("\nUnhandled tag ") + tagID + "\n";
  }

  return "";
}
