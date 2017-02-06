#ifndef wsn_flag_h
#define wsn_flag_h

#include <stdint.h>
#include <Arduino.h>

#ifndef WSN_NODE_TYPE_NODE
#define WSN_NODE_TYPE_NODE 0
#endif
#ifndef WSN_NODE_TYPE_HEAD_NODE
#define WSN_NODE_TYPE_HEAD_NODE 1
#endif
#ifndef WSN_NODE_TYPE_SINK
#define WSN_NODE_TYPE_SINK 3
#endif
#ifndef WSN_MESSAGE_TYPE_REQUEST
#define WSN_MESSAGE_TYPE_REQUEST 0
#endif
#ifndef WSN_MESSAGE_TYPE_RESPONSE
#define WSN_MESSAGE_TYPE_RESPONSE 1
#endif
#ifndef WSN_MESSAGE_STATUS_SUCCESS
#define WSN_MESSAGE_STATUS_SUCCESS 0
#endif
#ifndef WSN_MESSAGE_STATUS_ERROR
#define WSN_MESSAGE_STATUS_ERROR 1
#endif

//flgs -> xxyz0000 0000
// xx -> nodeType
// y -> messageType
// z -> messageStatus
struct WSNPacketFlags{
  uint8_t nodeType = 0;
  uint8_t messageType = 0;
  uint8_t messageStatus = 0;

  WSNPacketFlags()
  {
    nodeType = 0;
    messageType = 0;
    messageStatus = 0;
  };
  WSNPacketFlags(uint8_t nt, uint8_t mt, uint8_t ms)
  {
      nodeType = nt;
      messageType = mt;
      messageStatus = ms;
  };

  WSNPacketFlags(uint16_t flags)
  {
    setFromValue(flags);
  }

  void setFromValue(uint16_t flags)
  {
    //nodeType    xx000000 00000000
    //mask = 3 -> 00000000 00000011
    nodeType = 3 & (flags >> 14);

    //messageType 00z00000 00000000
    //mask = 1 -> 00000000 00000001
    messageType = 1 & (flags >> 13);

    //messageStatus 000y0000 00000000
    //mask = 1 ->   00000000 00000001
    messageStatus = 1 & (flags >> 12);
    //TODO make bit is set as a macro
  }

  uint16_t getValue()
  {
    //flags -> xxyz0000 0000
    // xx -> nodeType
    // y -> messageType
    // z -> messageStatus
    uint16_t value = 0;

    value = ((uint16_t)nodeType) << 14;
    value = ((uint16_t)messageType) << 13 | value;
    value = ((uint16_t)messageStatus) << 12 | value;

    return value;
  }

  String nodeTypeToString()
  {
    switch (nodeType)
    {
      case WSN_NODE_TYPE_NODE:
        return "Node";
      case WSN_NODE_TYPE_HEAD_NODE:
        return "HeadNode";
      case WSN_NODE_TYPE_SINK:
        return "Sink";
      default:
        return "Unknown";
    }
  }

  String messageTypeToString()
  {
    switch (messageType)
    {
      case WSN_MESSAGE_TYPE_REQUEST:
        return "Request";
      case WSN_MESSAGE_TYPE_RESPONSE:
        return "Response";
      default:
        return "Unknown";
    }
  }

  String messageStatusToString()
  {
    switch (messageStatus)
    {
      case WSN_MESSAGE_STATUS_SUCCESS:
        return "Success";
      case WSN_MESSAGE_STATUS_ERROR:
        return "Error";
      default:
        return "Unknown";
    }
  }

  String toString()
  {
    return String("NodeType: ") + nodeTypeToString() + "\n" +
      "MessageType: " + messageTypeToString() + "\n" +
      "MessageStatus: " + messageStatusToString() + "\n" +
      "Value:" + getValue();
  }
};

#endif //wsn_flag_h
