#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP
#include "AppIncludes.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
class RequestHandler
{
private:
    const int maxRequest = 50;
    int handledRequests = 0;
    bool isHandlerActive = false;

    SOCKET client_socket_fh;

    std::stringstream requestHeaderStream;
    std::stringstream requestBodyStream;
    std::unordered_map<std::string, std::string> requestHeadersMap;

    std::stringstream responseStream;

    const std::function<int(Request &req, Response &res)> &service;

public:
    /*
        initializes the map, client_socket_fh and service()
        invokes the handleReqRes function
    */
    RequestHandler(SOCKET client_socket, const std::function<int(Request &req, Response &res)> &service);

private:
    /*
        recieves request data and responds
        invokes requestParserHeader() parses the header
        invokes service function passed from HttpServer
    */
    int handleReqRes();

    /*
        Parses the request header and is stored in the reqmap
        returns the 'Content-Length' from the header-field
    */
    int requestParserHeader();

    // ill add detailed comments here later...

    // Checks the header if connection is set to keep alive
    bool isConnectionKeepAlive();

    // recieveing funtion
    bool startReciving();

    bool startSending();

    // clears all streams and header map
    bool clearOneReqResCycle();
};

#endif