#include "../ServerLibrary.h"
#include "./IOCP.h"

bool IOCPServer::Initialize() {
    // 윈속 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    // IOCP 핸들 생성
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocpHandle == NULL) {
        std::cerr << "CreateIoCompletionPort failed" << std::endl;
        return false;
    }

    // 리슨 소켓 설정
    if (!SetupListenSocket()) {
        return false;
    }

    // 워커 스레드 생성
    if (!CreateWorkerThreads()) {
        return false;
    }

    running = true;
    return true;
}

bool IOCPServer::SetupListenSocket() {
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(19990); // 서버 포트
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        return false;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }

    std::cout << "Listening on port 19990..." << std::endl;

    // 리슨 소켓을 IOCP에 연결
    if (!AssociateWithIOCP(listenSocket)) {
        return false;
    }

    return true;
}

bool IOCPServer::AssociateWithIOCP(SOCKET socket) {
    HANDLE result = CreateIoCompletionPort((HANDLE)socket, iocpHandle, (ULONG_PTR)socket, 0);
    if (result == NULL) {
        std::cerr << "AssociateWithIOCP failed" << std::endl;
        return false;
    }
    return true;
}

bool IOCPServer::CreateWorkerThreads() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = 2 * 2; // FIXME 스레드 생성 수 조절

    for (int i = 0; i < numThreads; ++i) {
        workerThreads.emplace_back(&IOCPServer::WorkerThread, this);
    }

    return true;
}

void IOCPServer::WorkerThread() {
    while (running) {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        OVERLAPPED* overlapped = nullptr;

        BOOL success = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        if (!success || bytesTransferred == 0) {
            std::cerr << "GetQueuedCompletionStatus failed" << std::endl;
            break;
        }

        // 완료된 입출력 처리 (클라이언트 통신 로직 추가)
        std::cout << "I/O operation completed" << std::endl;
    }
}

void IOCPServer::Run() {
    // 서버 실행 루프 (예: 클라이언트 수락 등 추가 가능)
    while (running)
    {

    }
}

void IOCPServer::Stop() {
    running = false;

    // 워커 스레드 종료
    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void IOCPServer::Cleanup() {
    Stop();

    // 소켓 정리
    if (listenSocket != INVALID_SOCKET) {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
    }

    // IOCP 핸들 정리
    if (iocpHandle != NULL) {
        CloseHandle(iocpHandle);
        iocpHandle = NULL;
    }

    WSACleanup();
}