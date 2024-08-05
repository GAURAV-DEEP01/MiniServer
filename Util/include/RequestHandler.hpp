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
    std::unordered_map<std::string, std::string> requestHeadersMap;

    std::stringstream responseStream;

    const std::function<int()> &service;

public:
    RequestHandler(SOCKET client_socket, const std::function<int()> &service);

private:
    int handleReqRes();
    int requestParserHeader();
};

#endif