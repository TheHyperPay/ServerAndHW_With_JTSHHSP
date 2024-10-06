#pragma once
#include "../ServerLibrary.h"
#include <atomic>

typedef UINT64					object_t;
typedef UINT64					oid_t;
typedef int32_t					packet_size_t;
typedef std::time_t				tick_t;
typedef std::thread				thread_t;
typedef std::thread::id			threadId_t;
typedef std::recursive_mutex	lock_t;
typedef std::string				str_t;
typedef std::wstring			wstr_t;

//tinyxml
typedef TiXmlDocument			xml_t;
typedef TiXmlElement			xmlNode_t;
typedef TiXmlHandle				xmlHandle_t;

#define SOCKET_BUF_SIZE			1024 * 8