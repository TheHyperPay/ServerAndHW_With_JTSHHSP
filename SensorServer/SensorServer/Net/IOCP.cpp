#include "../ServerLibrary.h"
#include "./IOCP.h"

bool IOCPServer::Initialize() {
    // ���� �ʱ�ȭ
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    // IOCP �ڵ� ����
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocpHandle == NULL) {
        std::cerr << "CreateIoCompletionPort failed" << std::endl;
        return false;
    }

    // ���� ���� ����
    if (!SetupListenSocket()) {
        return false;
    }

    // ��Ŀ ������ ����
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
    serverAddr.sin_port = htons(19990); // ���� ��Ʈ
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

    // ���� ������ IOCP�� ����
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
    int numThreads = 2 * 2; // FIXME ������ ���� �� ����

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

        // �Ϸ�� ����� ó�� (Ŭ���̾�Ʈ ��� ���� �߰�)
        std::cout << "I/O operation completed" << std::endl;
    }
}

void IOCPServer::Run() {
    // ���� ���� ���� (��: Ŭ���̾�Ʈ ���� �� �߰� ����)
    while (running)
    {

    }
}

void IOCPServer::Stop() {
    running = false;

    // ��Ŀ ������ ����
    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void IOCPServer::Cleanup() {
    Stop();

    // ���� ����
    if (listenSocket != INVALID_SOCKET) {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
    }

    // IOCP �ڵ� ����
    if (iocpHandle != NULL) {
        CloseHandle(iocpHandle);
        iocpHandle = NULL;
    }

    WSACleanup();
}