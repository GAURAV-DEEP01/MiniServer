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
    /*
        initializes the map, client_socket_fh and service()
        invokes the handleReqRes function
    */
    RequestHandler(SOCKET client_socket, const std::function<int()> &service);

private:
    /*
        recieves request data and responds
        invokes requestParserHeader() parses the header
        invokes service function passed from HttpServer
        
        implementing the service funciton in progress...
        implementing the response in progress... 
    */
    int handleReqRes();

    /*
        Parses the request header and is stored in the reqmap
        returns the 'Content-Length' from the header-field
    */
    int requestParserHeader();
};

#endif