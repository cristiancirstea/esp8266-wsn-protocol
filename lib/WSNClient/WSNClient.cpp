#include "WSNClient.h"

WSNClient::WSNClient()
{
}

WSNClient::WSNClient(char host[], int port, int nodeID)
{
  String h(host);
  WSNClient(h, port, nodeID);
}

WSNClient::WSNClient(String host, int port, int nodeID)
{
	this->setHost(host);
	this->setPortNumber(port);
	this->setNodeID(nodeID);
}

WSNClient::WSNClient(String host, int port, int nodeID, bool debug)
{
  WSNClient(host, port, nodeID);
  this->_DEBUG = debug;
}

WSNClient::WSNClient(char host[], int port, int nodeID, bool debug)
{
  WSNClient(host, port, nodeID);
  this->_DEBUG = debug;
}

/************ GETTER/SETTER ***************/
void WSNClient::setHost(String host)
{
	//TODO verify
	this->_host = host;
}

String WSNClient::getHost()
{
	return this->_host;
}

void WSNClient::setContentType(String ct)
{
	//TODO verify
	this->_contentType = ct;
}

String WSNClient::getContentType()
{
	return this->_contentType;
}

void WSNClient::setPortNumber(int port)
{
	//TODO verify
	this->_port = port;
}

int WSNClient::getPortNumber()
{
	return this->_port;
}

void WSNClient::setNodeID(int nodeID)
{
	//TODO verify
	this->_nodeID = nodeID;
}

int WSNClient::getNodeID()
{
	return this->_nodeID;
}

/************** PACKAGE ENCODING/DECODING ***************/
String WSNClient::encode(String message)
{
	return message;
}

String WSNClient::decode(String message)
{
	return message;
}

/******************* REQUEST *******************/

void WSNClient::validateMethod(String method)
{
  //TODO
}

void WSNClient::validateURI(String URI)
{
  //TODO
}

String WSNClient::request(String method, String URI)
{
  return this->request(method, URI, 0, NULL, "");
}

String WSNClient::request(String method, String URI, String body)
{
  return this->request(method, URI, 0, NULL, body);
}

String WSNClient::request(String method, String URI, int headersCount, WSN::HTTPHeader headers[])
{
  return this->request(method, URI, headersCount, headers, "");
}

String WSNClient::request(String method, String URI, int headersCount, WSN::HTTPHeader headers[], String body)
{
  int i;

	WSNClient::validateMethod(method);
	WSNClient::validateURI(URI);


  int port = this->getPortNumber();
  String host = this->getHost();

  char hostBuff[50];
//  hostBuff = (char*)malloc(sizeof(char) * (host.length() + 1) );
  host.toCharArray(hostBuff, host.length() + 1);

  this->logln("Connecting to", true);
  this->log(hostBuff, true);
  this->log(":");
  this->logln(port);

	if (!this->_client.connect(hostBuff, port))
	{
    //TODO
    this->logln("[ERROR] WSNClient -> Connection failed", true);
		return "";
	}

	String  message;

	message += method + " " + URI + " HTTP/1.1" + String(HTTP_HEADER_NEW_LINE);
	message += "Host: " + this->getHost() + ":" + String(this->getPortNumber()) + String(HTTP_HEADER_NEW_LINE);
	message += "Content-Type: " + this->getContentType() + String(HTTP_HEADER_NEW_LINE);
	message += "Content-Length: " + String(body.length()) + String(HTTP_HEADER_NEW_LINE);
  message += "WSN-Node-ID: " + String(this->getNodeID()) + String(HTTP_HEADER_NEW_LINE);

  for(i = 0; i < headersCount; i++)
  {
    message += headers[i].title + ":" + headers[i].value + String(HTTP_HEADER_NEW_LINE);
  }
  message += "Connection: " + this->_connectionType + String(HTTP_HEADER_END);

	if(body.length() > 0)
	{
		message += WSNClient::encode(body);
	}

  this->logln("Sending message: ", true);
  this->logln(message);
	this->_client.print(message);

  if(!this->isWaitingForResponse())
  {
    this->logln("Not waiting for response", true);
    return "";
  }

	String response;

  this->logln("Waiting for response", true);
	while(this->_client.available())
	{
		response += this->_client.readStringUntil('\r');
    this->log(".");
	}
	this->logln();

	return WSNClient::decode(response);
}

String WSNClient::getRequest(String URI)
{
  return this->request(REQUEST_METHOD_GET, URI);
}

String WSNClient::putRequest(String URI, String body)
{
  return this->request(REQUEST_METHOD_PUT, URI, body);
}

String WSNClient::postRequest(String URI, String body)
{
  return this->request(REQUEST_METHOD_POST, URI, body);
}

String WSNClient::deleteRequest(String URI, String body)
{
  return this->request(REQUEST_METHOD_DELETE, URI, body);
}

void WSNClient::enableWaitForRespose()
{
  this->_waitForResponse = true;
}

void WSNClient::disableWaitForRespose()
{
  this->_waitForResponse = false;
}

bool WSNClient::isWaitingForResponse()
{
  return this->_waitForResponse;
}

/************* DEBUG ******************/
void WSNClient::enableDebug()
{
  this->_DEBUG = true;
}

void WSNClient::disableDebug()
{
  this->_DEBUG = false;
}

void WSNClient::log(String message, bool signature)
{
  if(signature)
  {
    this->log(this->_logSignature);
  }
  this->log(message);
}

void WSNClient::log(String message)
{
  if(!this->_DEBUG)
  {
    return;
  }

  if(Serial)
  {
    Serial.print(message);
  }
}

void WSNClient::logln()
{
  this->logln("", false);
}

void WSNClient::logln(String message, bool signature)
{
  if(signature)
  {
    this->log(this->_logSignature);
  }
  this->logln(message);
}

void WSNClient::logln(String message)
{
   this->log(message + "\r\n");
}

void WSNClient::log(int number)
{
  this->log(String(number));
}

void WSNClient::log(int number, bool signature)
{
  this->log(String(number), signature);
}

void WSNClient::logln(int number)
{
  this->logln(String(number));
}

void WSNClient::logln(int number, bool signature)
{
  this->logln(String(number), signature);
}
