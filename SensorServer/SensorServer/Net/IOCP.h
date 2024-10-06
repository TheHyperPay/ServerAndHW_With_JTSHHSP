#pragma once
#include "../ServerLibrary.h"

#define IOCPSERVER      IOCPServer::getInstance()
#define MAX_IOCP_THREAD		64

class IOCPServer : public Singleton<IOCPServer>
{
public:
    IOCPServer() : iocpHandle(NULL), listenSocket(INVALID_SOCKET) {}
    ~IOCPServer() { Cleanup(); }

    bool Initialize();
    void Run();
    void Stop();
    void Cleanup();

private:
    HANDLE iocpHandle;              // IOCP �ڵ�
    SOCKET listenSocket;            // ���� ����
    std::vector<std::thread> workerThreads; // ��Ŀ �������
    bool running = false;

    void WorkerThread();             // ��Ŀ ������ �Լ�
    bool CreateWorkerThreads();      // ��Ŀ ������ ����
    bool SetupListenSocket();        // ���� ���� ����
    bool AssociateWithIOCP(SOCKET socket); // ������ IOCP�� ����

    friend class Singleton<IOCPServer>;
};