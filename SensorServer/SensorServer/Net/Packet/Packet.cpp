#include "../../ServerLibrary.h"
#include "Packet.h"


template<typename T>
string Packet<T>::toJson(const T packetData)
{
	return string();
}

template <>
PacketDataHeader* Packet<PacketDataHeader>::toStruct(const string jsonData)
{
	string err;
	Json json = Json::parse(jsonData, err);

	if (!err.empty()) {
		throw std::runtime_error("Error parsing JSON: " + err);
	}

	_packetData.packetType = json["packetType"].int_value();
	_packetData.deviceCode = json["deviceCode"].int_value();

	return &_packetData;
}

template <>
P_D_SENSOR_ACCEL_PacketData* Packet<P_D_SENSOR_ACCEL_PacketData>::toStruct(const string jsonData)
{
	string err;
	Json json = Json::parse(jsonData, err);

	if (!err.empty()) {
		throw std::runtime_error("Error parsing JSON: " + err);
	}

	_packetData.packetType = json["packetType"].int_value();
	_packetData.deviceCode = json["deviceCode"].int_value();
	_packetData.accelX = (float)json["accelX"].number_value();
	_packetData.accelY = (float)json["accelY"].number_value();
	_packetData.accelZ = (float)json["accelZ"].number_value();
	_packetData.pitch = (float)json["pitch"].number_value();
	_packetData.yaw = (float)json["yaw"].number_value();
	_packetData.roll = (float)json["roll"].number_value();

	return &_packetData;
}

template <>
P_M_DEVICE_CONNECT_PacketData* Packet<P_M_DEVICE_CONNECT_PacketData>::toStruct(const string jsonData)
{
	string err;
	Json json = Json::parse(jsonData, err);

	if (!err.empty()) {
		throw std::runtime_error("Error parsing JSON: " + err);
	}

	_packetData.packetType = json["packetType"].int_value();
	_packetData.deviceCode = json["deviceCode"].int_value();
	_packetData.matchDevice = json["matchDevice"].int_value();

	return &_packetData;
}
