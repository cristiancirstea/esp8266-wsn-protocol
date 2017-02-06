#ifndef NODE_ID
#define NODE_ID 1

#endif
#ifndef NODE_TYPE
#define NODE_TYPE WSN_NODE_TYPE_NODE
#endif
#ifndef NODE_LABEL
#define NODE_LABEL "HeadNode MCU L"
#endif
#ifndef WSN_NETWORK_TYPE
#define WSN_NETWORK_TYPE WSN_NETWORK_TYPE_MESH
#endif
#ifndef SENSOR_ENABLED
#define SENSOR_ENABLED true
#endif
#ifndef SENSOR_ID
#define SENSOR_ID 1
#endif
#ifndef SENSOR_TYPE
#define SENSOR_TYPE SENSOR_TYPE_GY_30
#endif
#ifndef SENSOR_LABEL
#define SENSOR_LABEL "Light"
#endif
#ifndef SENSOR_UNIT
#define SENSOR_UNIT "lux"
#endif
#ifndef NODE_SUBNET_SUFFIX
#define NODE_SUBNET_SUFFIX 108
#endif
//MUST contain WSN_SSID_NODE_PREFIX or WSN_SSID_HEAD_NODE_PREFIX
#ifndef NODE_AP_SSID
#define NODE_AP_SSID "HN-ESP8266-L"
#endif
#ifndef HEARTBEAT_UDP_PORT
#define HEARTBEAT_UDP_PORT 12345
#endif
#ifndef NODE_WIFI_MODE
#define NODE_WIFI_MODE WIFI_STA
#endif
#ifndef WSN_PACKET_ITEM_SERIAL_DEBUG
#define WSN_PACKET_ITEM_SERIAL_DEBUG true
#endif
#ifndef WSN_PACKET_SERIAL_DEBUG
#define WSN_PACKET_SERIAL_DEBUG true
#endif
#ifndef NODE_LED_SIGNAL_DELAY
#define NODE_LED_SIGNAL_DELAY 50 /* x2 -> before on and before off*/
#endif
#ifndef OLED_RESET_PIN
#define OLED_RESET_PIN D8 /* x2 -> before on and before off*/
#endif
#ifndef HEARTBEAT_INTERVAL_MS
#define HEARTBEAT_INTERVAL_MS 10000
#endif
