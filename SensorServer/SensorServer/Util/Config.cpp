#include "../ServerLibrary.h"
#include "Config.h"

bool loadConfig(xml_t* config)
{
	if (!config->LoadFile("../Resources/Config.xml"))
	{
		std::cout << "! Not exist config file." << std::endl;
		return false;
	}
	return true;
}