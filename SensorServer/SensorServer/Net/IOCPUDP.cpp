#include "../ServerLibrary.h"
#include "./IOCPUDP.h"

bool IOCPUDPServer::Initialize() 
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocpHandle == NULL) 
    {
        std::cerr << "CreateIoCompletionPort failed" << std::endl;
        return false;
    }

    if (!SetupUDPSocket()) 
    {
        return false;
    }

    if (!CreateWorkerThreads()) 
    {
        return false;
    }
    
    running = true;
    return true;
}

bool IOCPUDPServer::SetupUDPSocket() 
{
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0); // UDP 소켓 생성
    if (udpSocket == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(19991);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) 
    {
        std::cerr << "Bind failed" << std::endl;
        return false;
    }

    if (!AssociateWithIOCP(udpSocket)) 
    {
        return false;
    }

    // 첫 번째 비동기 수신 요청을 게시
    PerIoData* perIoData = new PerIoData;
    ZeroMemory(&perIoData->overlapped, sizeof(WSAOVERLAPPED));
    perIoData->addrLen = sizeof(sockaddr_in);

    return PostReceive(perIoData);
}

bool IOCPUDPServer::AssociateWithIOCP(SOCKET socket) 
{
    HANDLE result = CreateIoCompletionPort((HANDLE)socket, iocpHandle, (ULONG_PTR)socket, 0);
    if (result == NULL) 
    {
        std::cerr << "AssociateWithIOCP failed" << std::endl;
        return false;
    }
    return true;
}

//비동기적으로 데이터를 받기 위한 준비 작업
bool IOCPUDPServer::PostReceive(PerIoData* perIoData) 
{
    perIoData->wsaBuf.buf = perIoData->buffer;
    perIoData->wsaBuf.len = sizeof(perIoData->buffer);

    DWORD flags = 0;
    DWORD bytesReceived = 0;
    int result = WSARecvFrom(udpSocket, &perIoData->wsaBuf, 1, &bytesReceived, &flags,
        (sockaddr*)&perIoData->clientAddr, &perIoData->addrLen,
        &perIoData->overlapped, NULL);
    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) 
    {
        std::cerr << "WSARecvFrom failed" << std::endl;
        return false;
    }

    return true;
}

bool IOCPUDPServer::SendData(PerIoData* perIoData, const char* message)
{
    // 클라이언트에게 메시지 전송
    sockaddr_in clientAddr = perIoData->clientAddr; // 클라이언트 주소 정보
    int addrLen = sizeof(clientAddr);
    int result = sendto(udpSocket, message, strlen(message), 0, (sockaddr*)&clientAddr, addrLen);

    if (result == SOCKET_ERROR)
    {
        std::cerr << "sendto failed" << std::endl;
        return false;
    }

    std::cout << "Sent message to client: " << message << std::endl;
    return true;
}

bool IOCPUDPServer::CreateWorkerThreads() 
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = 2 * 2;

    for (int i = 0; i < numThreads; ++i) 
    {
        workerThreads.emplace_back(&IOCPUDPServer::WorkerThread, this);
    }

    return true;
}

//WorkerThread가 I/O 완료 통지를 받고, 해당 작업을 처리하는 방식으로 동작
void IOCPUDPServer::WorkerThread() 
{
    while (running) {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        OVERLAPPED* overlapped = nullptr;

        BOOL success = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        if (!success || bytesTransferred == 0) 
        {
            std::cerr << "GetQueuedCompletionStatus failed" << std::endl;
            continue;
        }

        // 완료된 I/O 작업 처리
        PerIoData* perIoData = (PerIoData*)overlapped;

        cout << "Get Message: " << perIoData->buffer << endl;

        SendData(perIoData, perIoData->buffer);

        //buffer에서 packet처리하기
        /*
        Packet<P_D_SENSOR_ACCEL_PacketData>* packet = new Packet<P_D_SENSOR_ACCEL_PacketData>();
        packet->toStruct(perIoData->buffer);

        cout << "Get Message>> "
            << " PT: " << packet->getPacketData()->packetType
            << " DC: " << packet->getPacketData()->deviceCode
            << " AX: " << packet->getPacketData()->accelX
            << " AY: " << packet->getPacketData()->accelY
            << " AZ: " << packet->getPacketData()->accelZ
            << " PI: " << packet->getPacketData()->pitch
            << " YA: " << packet->getPacketData()->yaw
            << " RO: " << packet->getPacketData()->roll
            << endl;

        SAFE_DELETE(packet)
        */

        memset(perIoData->buffer, 0, sizeof(perIoData->buffer));

        // 데이터 처리 후 다시 수신 요청 게시
        PostReceive(perIoData);
    }
}

void IOCPUDPServer::Run() 
{
    std::cout << "UDP Server running..." << std::endl;

    while (running) 
    {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        OVERLAPPED* overlapped = nullptr;

        BOOL success = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        if (!success || bytesTransferred == 0) 
        {
            std::cerr << "GetQueuedCompletionStatus failed" << std::endl;
            continue;
        }

        // 완료된 I/O 작업 처리
        PerIoData* perIoData = (PerIoData*)overlapped;

        // 수신된 메시지 출력
        std::cout << "Received message from client: " << perIoData->buffer << std::endl;

        // 다시 수신 요청 게시
        PostReceive(perIoData);
    }
}

void IOCPUDPServer::Stop() 
{
    running = false;

    for (auto& thread : workerThreads) 
    {
        if (thread.joinable()) 
        {
            thread.join();
        }
    }
}

void IOCPUDPServer::Cleanup() 
{
    Stop();

    if (udpSocket != INVALID_SOCKET) 
    {
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;
    }

    if (iocpHandle != NULL) 
    {
        CloseHandle(iocpHandle);
        iocpHandle = NULL;
    }

    WSACleanup();
}