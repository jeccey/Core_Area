#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
/**
	C++ client example using sockets
*/
#include<iostream>	//cout
#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<string>	//string
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include<netdb.h>	//hostent

#include "Message_handler.h"
#include "XML_Parse.h"

using namespace std;

/**
	TCP Client class
*/
class TCP_Client
{
private:
	int sock;
	std::string address;
	int port;
	struct sockaddr_in server;

public:
    Message_handler *msghdler;
    struct sip_message sip_msg;
    DealMessage dealMsg;

	TCP_Client();
	virtual ~TCP_Client();
	bool conn(string, int);
	bool send_data(const char* data,  int numBytes);
	void register_xmlparese();
	string receive(int);
};

#endif //TCP_CLIENT_H
