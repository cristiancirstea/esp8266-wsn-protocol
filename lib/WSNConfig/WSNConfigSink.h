#ifndef wsn_config_sink_h
#define wsn_config_sink_h

struct WSNConfigSink{
  // const char* APIHost = "178.62.104.160"; //cloud
  const char* APIHost = "192.168.44.100";
  int APIPort = 1337;
  // const char* UDPHost = "178.62.104.160"; //cloud
  const char* UDPHost = "192.168.44.100";
  int UDPPort = 33333;
};

#endif
