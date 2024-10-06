#include "./ServerLibrary.h"

int main()
{
    if (!IOCPUDP.Initialize())
    {
        std::cerr << "Server initialization failed." << std::endl;
        return -1;
    }

    std::cout << "Server initialized and running..." << std::endl;

    IOCPUDP.Run();

    IOCPUDP.Cleanup();

    return 0;
} 