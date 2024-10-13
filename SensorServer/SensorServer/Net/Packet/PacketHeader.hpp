#pragma once
#include "../../ServerLibrary.h"
#include "Packet.h"

typedef uint8_t		hash_t;

enum PacketType : UInt32 {
	P_E_ERROR_T				= 1000,
	P_D_SENSOR_ACCEL_T		= 1001,
	P_M_DEVICE_CONNECT_T	= 1002
};

struct PacketDataBase {
	int packetType; int deviceCode;
};

struct P_D_SENSOR_ACCEL_PacketData : public PacketDataBase {
	float accelX;	float accelY;	float accelZ;
	float pitch;	float yaw;		float roll;
};

struct P_M_DEVICE_CONNECT_PacketData : public PacketDataBase {
	int matchDevice;
};
