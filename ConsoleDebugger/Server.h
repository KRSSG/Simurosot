#pragma once
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
class server
{
public:
	server(void);
	~server(void);
	SOCKET SocketServer; 
	void RecieveMessage();
};