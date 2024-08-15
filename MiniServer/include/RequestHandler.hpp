#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP
#include "AppIncludes.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
class RequestHandler
{
private:
    const int maxRequest = 100;
    const int maxTimout = 10000; // seconds
    int handledRequests = 0;

    std::atomic<bool> isHandlerActive;

    const SOCKET client_socket_fh;
    const sockaddr_in server_addr;
    std::string ipString;

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
    RequestHandler(SOCKET client_socket,
                   sockaddr_in server_addr,
                   const std::function<int(Request &req, Response &res)> &service);

    ~RequestHandler();

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

    void monitorClientInactivity();

    bool startReciving();

    bool startSending();

    // clears all streams and header map
    bool clearOneReqResCycle();
};

#endif