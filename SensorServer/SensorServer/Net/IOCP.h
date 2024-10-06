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
    HANDLE iocpHandle;              // IOCP 핸들
    SOCKET listenSocket;            // 리슨 소켓
    std::vector<std::thread> workerThreads; // 워커 스레드들
    bool running = false;

    void WorkerThread();             // 워커 스레드 함수
    bool CreateWorkerThreads();      // 워커 스레드 생성
    bool SetupListenSocket();        // 리슨 소켓 설정
    bool AssociateWithIOCP(SOCKET socket); // 소켓을 IOCP에 연결

    friend class Singleton<IOCPServer>;
};