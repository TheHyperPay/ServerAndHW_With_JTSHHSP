#pragma once
#include "../../ServerLibrary.h"
#include "./PacketHeader.hpp"

template <typename T>
class Packet{
private:
	T _packetData;

public:
	string toJson(const T packetData);
	T* toStruct(const string jsonData);
	T* getPacketData() { return &_packetData; }
};
