#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP
#include "AppIncludes.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

typedef enum
{
    REQUEST_SAFE_STATE,
    REQUEST_LIMIT_EXCEEDED,
    REQUEST_PROCESSING_ERROR,
    REQUEST_HEADER_PARSE_ERROR,
    REQUEST_SERVICE_ERROR,
    REQUEST_RECIEVE_FAILED,
    REQUEST_TIMEOUT_OR_FAILED,
    REQUEST_CONNECTION_CLOSED
} REQUESTCONST;

class RequestHandler
{
private:
    const int maxRequest = 100;
    const int maxTimout = 15000; // milliseconds
    int handledRequests = 0;

    const SOCKET client_socket_fh;
    const sockaddr_in server_addr;
    std::string ipString;

    std::stringstream requestHeaderStream;
    std::stringstream requestBodyStream;
    std::unordered_map<std::string, std::string> requestHeadersMap;

    std::vector<unsigned char> responseBuffer;

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

    // Checks the header if Connection header exists and if Connection is set to keep alive
    bool isConnectionKeepAlive();

    // recv() is invoked here
    int startReciving();

    /*
        send() is invoked here
        headerData is stored in 'requestHeadersMap'
    */
    bool startSending();

    // clears all streams and header map and increments the handledRequest counter
    bool clearOneReqResCycle();
};

#endif