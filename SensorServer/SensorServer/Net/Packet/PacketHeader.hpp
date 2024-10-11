#pragma once

typedef uint8_t		hash_t;

enum PacketType : UInt32 {
	P_E_ERROR_T				= 1000,
	P_D_SENSOR_ACCEL_T		= 1001,
	P_M_DEVICE_CONNECT_T	= 1002
};

struct P_D_SENSOR_ACCEL_PacketData {
	int packetType; int deviceCode;
	float accelX;	float accelY;	float accelZ;
	float pitch;	float yaw;		float roll;
};