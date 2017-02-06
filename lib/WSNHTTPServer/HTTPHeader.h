#ifndef wsn_http_header_h
#define wsn_http_header_h

#include <Arduino.h>

namespace WSN
{
  struct HTTPHeader {
    String title;
    String value;

    HTTPHeader(String t, String v):title(t),value(v){}
  };
}

#endif //wsn_http_header_h
