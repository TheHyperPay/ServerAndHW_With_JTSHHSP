#pragma once
#include "../../ServerLibrary.h"
#include "./PacketHeader.hpp"

class PacketBase {

};

template <typename T>
class Packet : public PacketBase{
private:
	T _packetData;
public:
	string toJson(const T packetData);
	T* toStruct(const string jsonData);
	T* getPacketData() { return &_packetData; }
};