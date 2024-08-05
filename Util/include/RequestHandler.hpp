#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP
#include "AppIncludes.hpp"
class RequestHandler
{
private:
    int name, vers;
    SOCKET client_socket_fh;

    std::stringstream requestHeaderStream;

    std::stringstream requestBodyStream;

    std::stringstream responseStream;
    std::unordered_map<std::string, std::string> requestHeadersMap;

public:
    RequestHandler(SOCKET client_socket);

private:
    int handleReqRes();
    int requestParserHeader();
};

#endif