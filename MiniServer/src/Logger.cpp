#include "../include/Logger.hpp"

static std::mutex loggerAccess;

namespace Logger
{
    void err(const std::string &errMsg, SOCKET socket_fh)
    {
        loggerAccess.lock();
        std::cerr << "Error: " << errMsg << "!" << std::endl;
        std::cerr << "Error code: " << WSAGetLastError() << std::endl;
        if (socket_fh != INVALID_SOCKET)
            closesocket(socket_fh);
        loggerAccess.unlock();
    }

    void status(const std::string &statusMsg)
    {
        std::cout << statusMsg << "..." << std::endl;
    }

    void info(const std::string &infoMsg)
    {
        loggerAccess.lock();
        std::cout << "\n\nINFO: ---------------\n\n"
                  << infoMsg << "\n\n"
                  << "---------------------\n"
                  << std::endl;
        loggerAccess.unlock();
    }

    void logs(const std::string &log)
    {
        time_t curr_time;
        struct tm *curr_tm;
        char time_string[100];
        time(&curr_time);
        curr_tm = localtime(&curr_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", curr_tm);

        loggerAccess.lock();
        std::clog << "{ '" << log << "', time: " << time_string << " }" << std::endl;
        loggerAccess.unlock();
    }
}