#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "Winmm.lib")

#include <winsock2.h>

#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <string>
#include <vector>
#include <tchar.h>
#include <regex>

#include <thread>
#include <mutex>
#include <memory>

#include <ctime>
#include <random>
#include <typeinfo>

#include "./tinyxml/tinyxml.h"

#if _DEBUG
#define CONTEXT_SWITCH		Sleep(1)
#else
#define CONTEXT_SWITCH		::SwitchToThread()
#endif

typedef void(*Function)(void*);

using namespace std;

#include "./Util/Type.hpp"
#include "./Util/Util.hpp"
#include "./Util/Singleton.hpp"
#include "./Util/Config.h"

#include "./Net/Server.h"
#include "./Net/IOCP.h"
#include "./Net/IOCPUDP.h"