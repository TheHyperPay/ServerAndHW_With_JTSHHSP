#include "../ServerLibrary.h"
#include "Server.h"

Server::Server()
{
	printf("Initialize Server");

	xml_t config;
	if (!loadConfig(&config))
	{
		return;
	}
	this->initialize(&config);
}
Server::~Server()
{
	_status = SERVER_STOP;
}

void Server::initialize(xml_t* config)
{
	xmlNode_t* root = config->FirstChildElement("App")->FirstChildElement("Server");
	if (!root)
	{
		printf("ERROR initialize");
		return;
	}

	xmlNode_t* elem = root->FirstChildElement("IP");
	strcpy_s(_ip, elem->GetText());

	elem = root->FirstChildElement("Port");
	sscanf_s(elem->GetText(), "%d", &_port);

	elem = root->FirstChildElement("ThreadCount");
	sscanf_s(elem->GetText(), "%d", &_workThreadCount);

	_status = SERVER_INITIALIZE;
}

SERVER_STATUS& Server::status()
{
	return _status;
}