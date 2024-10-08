#pragma once

#ifdef ENABLE_LOGGER_INFO
#define INFO(message) Logger::info(message)
#else
#define INFO(message)
#endif

#include "AppIncludes.hpp"

namespace Logger
{
    /*
        Prints error msg with WSA Last error error status code
        Error: msg
        Error code: WSA LAST ERROR CODE
    */
    void err(const std::string &errMsg, SOCKET socket_fh = INVALID_SOCKET);

    /*
        Prints status
        msg...
    */
    void status(const std::string &statusMsg);

    /*
        Prints info msg
        INFO ------------------------
        msg
        -----------------------------
    */
    void info(const std::string &infoMsg);

    // connection logs { msg : time }
    void logs(const std::string &log);
}
