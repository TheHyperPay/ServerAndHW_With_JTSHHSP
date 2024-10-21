#pragma once
#include "../ServerLibrary.h"

#define MOBILEMANAGER     MobileManager::getInstance()

typedef struct ClientInfo_t {
	string id = "NULL";
	sockaddr_in socket;
	int mobileDeviceCode = 0;
	int outerDeviceCode = 0;
	steady_clock::time_point lastActiveTime = steady_clock::now();  // ������ Ȱ��ȭ �ð�
	bool isActive = true;  // Ŭ���̾�Ʈ ���� (Ȱ��/��Ȱ��)
}ClientInfo_t;

class MobileManager : public Singleton<MobileManager>
{
private:
	vector<ClientInfo_t> clients;
	lock_t clientsLock;
	const duration<int> sessionTimeout = seconds(5);

public:
	void addClient(const sockaddr_in& clientAddr, const string& id, int mobileCode, int outerCode);
	ClientInfo_t* searchClientById(const string& id);
	ClientInfo_t* searchClientBySocket(const sockaddr_in& clientAddr);
	bool removeClient(const string& id);
	void cleanupExpiredSessions();

	//HACK: �ϴ� �������� �ϱ� ���� ������ �ڵ�
	sockaddr_in uniUser;
};