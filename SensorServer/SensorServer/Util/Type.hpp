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

typedef UINT64					UInt64;
typedef INT64					Int64;
typedef UINT32					UInt32;
typedef INT32					Int32;
typedef UINT16					UInt16;
typedef UINT8					UInt8;

//tinyxml
typedef TiXmlDocument			xml_t;
typedef TiXmlElement			xmlNode_t;
typedef TiXmlHandle				xmlHandle_t;

//json11
using namespace json11;

using namespace chrono;

#define SOCKET_BUF_SIZE			64 * 8