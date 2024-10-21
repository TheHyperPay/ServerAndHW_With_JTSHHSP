#pragma once
#include "../../ServerLibrary.h"
#include "Packet.h"

//json으로 받을 때 class 형태로 전부 분류해서 override가 더 간편한 구조
typedef uint8_t		hash_t;

enum PacketType : UInt32 {
	P_E_ERROR_T				= 1000,
	P_D_SENSOR_ACCEL_T		= 1001,
	P_M_DEVICE_CONNECT_T	= 1002
};

struct PacketDataHeader {
	int packetType = 1000; 
	int deviceCode = 0;
};

struct P_D_SENSOR_ACCEL_PacketData : public PacketDataHeader {
	float accelX = 0;	float accelY = 0;	float accelZ = 0;
	float pitch = 0;	float yaw = 0;		float roll = 0;
};

struct P_M_DEVICE_CONNECT_PacketData : public PacketDataHeader {
	int matchDevice = 0;
};

