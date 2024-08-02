#ifndef LOGGER_CPP
#define LOGGER_CPP

#include "AppIncludes.hpp"

namespace Logger
{
    // Prints error msg with WSL error status code
    void err(std::string errMsg, SOCKET socket_fh = INVALID_SOCKET);
    // Prints status msg
    void status(std::string statusMsg);
    // Prints infot msg
    void info(std::string infoMsg);
    // connection logs
    void logs(std::string log);
}

#endif