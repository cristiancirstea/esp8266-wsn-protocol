#ifndef wsnclient_h
#define wsnclient_h
#include <Arduino.h>
#include <WiFiClient.h>
#include "HTTPHeader.h"

#define REQUEST_METHOD_GET "GET"
#define REQUEST_METHOD_POST "POST"
#define REQUEST_METHOD_PUT "PUT"
#define REQUEST_METHOD_DELETE "DELETE"

#define HTTP_HEADER_NEW_LINE "\r\n"
#define HTTP_HEADER_END "\r\n\r\n"

class WSNClient {
	private :
		String _host;
		int _port;
		int _nodeID;
		String _contentType = "application/json";
		String _connectionType = "close";
    bool _DEBUG;
    bool _waitForResponse = true;

    const char* _logSignature = "[WSNClient] ";
		WiFiClient _client;
  public:
		WSNClient();
		WSNClient(String host, int port, int nodeID);
    WSNClient(char host[], int port, int nodeID);
    WSNClient(String host, int port, int nodeID, bool debug);
    WSNClient(char host[], int port, int nodeID, bool debug);

    String request(String method, String URI);
		String request(String method, String URI, String body);
    String request(String method, String URI, int headersCount, WSN::HTTPHeader headers[]);
    String request(String method, String URI, int headersCount, WSN::HTTPHeader headers[], String body);
		String getRequest(String URI);
		String putRequest(String URI, String body);
		String postRequest(String URI, String body);
		String deleteRequest(String URI, String body);

    /* Getters and Setters */
		void setHost(String host);
		String getHost();
		void setPortNumber(int port);
		int getPortNumber();
		void setNodeID(int nodeID);
		int getNodeID();
		void setContentType(String ct);
		String getContentType();
    void enableDebug();
    void disableDebug();
    bool isWaitingForResponse();
    void enableWaitForRespose();
    void disableWaitForRespose();

    void log(String message);
    void log(String message, bool signature);
    void log(int number);
    void log(int number, bool signature);
    void logln();
    void logln(String message);
    void logln(String message, bool signature);
    void logln(int number);
    void logln(int number, bool signature);

		static String encode(String message);
		static String decode(String message);

		static void validateMethod(String method);
		static void validateURI(String URI);

};
#endif
