#ifndef wsn_packet_item_h
#define wsn_packet_item_h

#include <stdint.h>
#include <Arduino.h>
//may be overrided in Node.h
#ifndef WSN_PACKET_ITEM_SERIAL_DEBUG
#define WSN_PACKET_ITEM_SERIAL_DEBUG false

#endif

namespace WSN
{
  const uint8_t PACKET_ITEM_TAG_SIZE_BYTES = 4; //bytes
  const uint8_t PACKET_ITEM_LENGTH_SIZE_BYTES = 4; //bytes
  const uint8_t PACKET_ITEM_ALLOWED_TAGS_LENGTH = 20;

  //!!!!!!!!!! TODO WHEN ADDING NEW TAGS, ADD THEM TO ALLOWED LIST !!!!!!!!!
  const uint32_t PACKET_TAG_NODE_ID = 0x0001;
  const uint32_t PACKET_TAG_NODE_LABEL = 0x0002;
  const uint32_t PACKET_TAG_NODE_IP = 0x0003;
  const uint32_t PACKET_TAG_NODE_PORT = 0x0004;
  const uint32_t PACKET_TAG_NODE_TYPE = 0x0005;
  const uint32_t PACKET_TAG_NODE_WIFI_MODE = 0x0006;
  const uint32_t PACKET_TAG_NODE_AP_SSID = 0x0007;
  const uint32_t PACKET_TAG_NODE_AP_PASSWORD = 0x0008;

  const uint32_t PACKET_TAG_SENSOR_ID = 0x0009;
  const uint32_t PACKET_TAG_SENSOR_LABEL = 0x000A; //10
  const uint32_t PACKET_TAG_SENSOR_VALUE = 0x000B; //11
  const uint32_t PACKET_TAG_SENSOR_TYPE = 0x000C; //12

  const uint32_t PACKET_TAG_ERROR_ID = 0x000D; //13
  const uint32_t PACKET_TAG_ERROR_MESSAGE = 0x000E; //14

  const uint32_t PACKET_TAG_BATTERY_LEVEL = 0x000F; //15

  const uint32_t PACKET_TAG_CUSTOM_MESSAGE = 0x0010; //16

  const uint32_t PACKET_TAG_NETWORK_TYPE = 0x0011; //17

  const uint32_t PACKET_TAG_PARENT_NODE_ID = 0x0012; //18
  const uint32_t PACKET_TAG_SCAN_WIFI_SSID = 0x0013; //19
  const uint32_t PACKET_TAG_SCAN_WIFI_DISTANCE = 0x0014; //20

}

/*
* TLV
* tag - 4 bytes
* length - 4 bytes
* Value - N bytes
*/
class WSNPacketItem
{
  private:
    uint32_t _tag;
    uint32_t _length;
    char* _value;
    void _setLength(int);
    bool _validateTag(uint32_t tag);

  public:
    uint32_t allowedTags[WSN::PACKET_ITEM_ALLOWED_TAGS_LENGTH] = {
      WSN::PACKET_TAG_NODE_ID,
      WSN::PACKET_TAG_NODE_LABEL,
      WSN::PACKET_TAG_NODE_IP,
      WSN::PACKET_TAG_NODE_PORT,
      WSN::PACKET_TAG_NODE_TYPE,
      WSN::PACKET_TAG_NODE_WIFI_MODE,
      WSN::PACKET_TAG_NODE_AP_SSID,
      WSN::PACKET_TAG_NODE_AP_PASSWORD,
      WSN::PACKET_TAG_SENSOR_ID,
      WSN::PACKET_TAG_SENSOR_LABEL,
      WSN::PACKET_TAG_SENSOR_VALUE,
      WSN::PACKET_TAG_SENSOR_TYPE,
      WSN::PACKET_TAG_ERROR_ID,
      WSN::PACKET_TAG_ERROR_MESSAGE,
      WSN::PACKET_TAG_BATTERY_LEVEL,
      WSN::PACKET_TAG_CUSTOM_MESSAGE,
      WSN::PACKET_TAG_NETWORK_TYPE,
      WSN::PACKET_TAG_PARENT_NODE_ID,
      WSN::PACKET_TAG_SCAN_WIFI_SSID,
      WSN::PACKET_TAG_SCAN_WIFI_DISTANCE
    };

    WSNPacketItem();
    WSNPacketItem(uint32_t tag, const char* value);
    WSNPacketItem(uint32_t tag, uint8_t* value, uint16_t size);
    WSNPacketItem(uint32_t tag, int32_t value);
    WSNPacketItem(uint32_t tag, float value);
    WSNPacketItem(WSNPacketItem &);
    ~WSNPacketItem();

    uint32_t getTag();
    bool setTag(uint32_t tag);
    uint32_t getLength();
    uint32_t getTotalLengthBytes();
    char* getValue();
    uint32_t getValueUInt32();
    uint16_t getValueUInt16();
    uint8_t getValueUInt8();
    bool setValue(const char* value);
    bool setValue(const uint8_t* value, uint16_t size);
    bool setValue(int32_t value);
    bool setValue(float value);
    void setValueNull();
    bool isValueNull();

    const String toString();
    const String valueToString();

    const char* serialize();

    static WSNPacketItem* deserialize(char* itemData, unsigned itemDataSize);

    static String tagToString(uint16_t tagID);
};

#endif //wsn_packet_item_h
