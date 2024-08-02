#include "../include/Logger.hpp"

namespace Logger
{
    void err(std::string errMsg, SOCKET socket_fh)
    {
        std::cerr << "Error: " << errMsg << "!" << std::endl;
        std::cerr << "Error code: " << WSAGetLastError() << std::endl;
        if (socket_fh != INVALID_SOCKET)
            closesocket(socket_fh);
        WSACleanup();
    }

    void status(std::string statusMsg)
    {
        std::cout << statusMsg << "..." << std::endl;
    }

    void info(std::string infoMsg)
    {
        std::cout << "\nINFO: ---------------\n"
                  << infoMsg << "\n"
                  << "---------------------" << std::endl;
    }

    void logs(std::string log)
    {
        time_t curr_time;
        struct tm *curr_tm;
        char time_string[100];
        time(&curr_time);
        curr_tm = localtime(&curr_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", curr_tm);

        std::cout << "{ '" << log << "' , time: " << time_string << " }" << std::endl;
    }
}