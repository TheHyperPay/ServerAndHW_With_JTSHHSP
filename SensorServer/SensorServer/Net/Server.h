#pragma once
#include "../ServerLibrary.h"

typedef enum SERVER_STATUS{
	SERVER_STOP = 0x01,
	SERVER_INITIALIZE,
	SERVER_READY
}SERVER_STATUS;

class Server
{
private:
	char			_ip[16] = "";
	int				_port = 0;
	int				_workThreadCount = 0;

	SERVER_STATUS	_status = SERVER_STOP;

public:
	Server();
	~Server();

	virtual void initialize(xml_t* config);
	//virtual bool run() = 0;

	SERVER_STATUS& status();
};