#include "../ServerLibrary.h"
#include "./MobileManager.h"

void MobileManager::addClient(const sockaddr_in& clientAddr, const string& id, int mobileCode, int outerCode)
{
    lock_guard<recursive_mutex> lock(clientsLock);
    ClientInfo_t newClient;
    newClient.socket = clientAddr;
    newClient.id = id;
    newClient.mobileDeviceCode = mobileCode;
    newClient.outerDeviceCode = outerCode;
    newClient.lastActiveTime = steady_clock::now();
    clients.push_back(newClient);
}

ClientInfo_t* MobileManager::searchClientById(const string& id)
{
    lock_guard<recursive_mutex> lock(clientsLock);
    for (auto& client : clients) {
        if (client.id == id) {
            client.lastActiveTime = steady_clock::now();  // 검색 시 마지막 활동 시간 갱신
            return &client;
        }
    }
    return nullptr;
}

ClientInfo_t* MobileManager::searchClientBySocket(const sockaddr_in& clientAddr)
{
    lock_guard<recursive_mutex> lock(clientsLock);
    for (auto& client : clients) {
        if (client.socket.sin_addr.s_addr == clientAddr.sin_addr.s_addr &&
            client.socket.sin_port == clientAddr.sin_port) {
            client.lastActiveTime = steady_clock::now();
            return &client;
        }
    }
    return nullptr;
}

bool MobileManager::removeClient(const string& id)
{
    lock_guard<recursive_mutex> lock(clientsLock);
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it->id == id) {
            clients.erase(it);
            return true;
        }
    }
    return false;
}

void MobileManager::cleanupExpiredSessions()
{
    lock_guard<recursive_mutex> lock(clientsLock);
    auto now = steady_clock::now();
    clients.erase(
        remove_if(clients.begin(), clients.end(), [now, this](const ClientInfo_t& client) {
            return (now - client.lastActiveTime > sessionTimeout);
            }),
        clients.end()
    );
}
