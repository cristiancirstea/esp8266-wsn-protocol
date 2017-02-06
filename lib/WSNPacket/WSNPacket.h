#ifndef wsn_packet_h
#define wsn_packet_h

#include <stdint.h>
#include "WSNPacketItem.h"
#include "WSNFlags.h"
//may be overrided in Node.h
#ifndef WSN_PACKET_SERIAL_DEBUG
#define WSN_PACKET_SERIAL_DEBUG false
#endif

#define WSN_PACKET_MAX_ATTRIBUTE_COUNT 100

namespace WSN
{
  const uint32_t PACKET_FUNCTION_DISCOVER = 0x0001;
  const uint32_t PACKET_FUNCTION_HEARTBEAT = 0x0002;
  const uint32_t PACKET_FUNCTION_REGISTER = 0x0003;
  const uint32_t PACKET_FUNCTION_CONFIG = 0x0004;
  const uint32_t PACKET_FUNCTION_QUERY = 0x0005;
  const uint32_t PACKET_FUNCTION_BROADCAST_MESSAGE = 0x0006;

  const uint8_t PACKET_HEADER_SIZE_BYTES = 16;
}

struct WSNPacketHeader{
  uint16_t version = 1; //2 bytes
  uint16_t functionID = 0; //2 bytes
  uint16_t nodeID = 0; //2 bytes
  uint16_t destinationNodeID = 0; //2 bytes
  uint16_t transactionID = 0; //2 bytes
  uint16_t flags = 0; //2 bytes //WSNFlags->getValue()
  uint16_t attributeCount = 0; //2bytes
  uint16_t packetDataSizeBytes = 0; //2 bytes
};

class WSNPacket
{
  private:
    WSNPacketHeader _header;
    unsigned _itemCount = 0;
    WSNPacketItem* _items;

    bool _validateHeader(WSNPacketHeader);
  public:
    WSNPacket(unsigned itemCount);
    WSNPacket(unsigned itemCount, WSNPacketItem* items);
    ~WSNPacket();

    WSNPacketHeader getHeader();
    bool setHeader(WSNPacketHeader);
    bool setHeader(uint16_t nodeID, uint16_t functionID, uint16_t destNodeID, uint16_t flags);
    uint32_t getTotalLengthBytes();

    WSNPacketItem* getItem(unsigned id);
    WSNPacketItem* getItemByTag(uint32_t tagID);
    unsigned getItemCount();
    bool setItem(unsigned id, WSNPacketItem* item);

    bool validate();
    String toString();
    const char* serialize();

    static WSNPacket* deserialize(char* dataBuffer, unsigned sizeBytes);
    static String functionIDToString(uint16_t functionID);
};

#endif //wsn_packet_h
