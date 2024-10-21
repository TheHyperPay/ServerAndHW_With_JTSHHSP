#pragma once
#include "../ServerLibrary.h"

#define IOCPUDP      IOCPUDPServer::getInstance()

class IOCPUDPServer : public Singleton<IOCPUDPServer>
{
public:
    IOCPUDPServer() : iocpHandle(NULL), udpSocket(INVALID_SOCKET) {}
    ~IOCPUDPServer() { Cleanup(); }

    bool Initialize();
    void Run();
    void Stop();
    void Cleanup();

private:
    HANDLE iocpHandle;
    SOCKET udpSocket;
    std::vector<std::thread> workerThreads;
    bool running = false;

    //작업 관리 추적용
    struct PerIoData {
        WSAOVERLAPPED overlapped;
        WSABUF wsaBuf;
        char buffer[SOCKET_BUF_SIZE] = { 0 };
        sockaddr_in clientAddr;
        int addrLen;
    };

    void WorkerThread();
    bool CreateWorkerThreads();
    bool SetupUDPSocket();
    bool AssociateWithIOCP(SOCKET socket);
    bool PostReceive(PerIoData* perIoData);
    bool SendData(sockaddr_in client, const char* message);
};