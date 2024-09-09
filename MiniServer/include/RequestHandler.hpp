#pragma once

#include "AppIncludes.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

typedef enum
{
    REQUEST_SAFE_STATE,
    REQUEST_LIMIT_EXCEEDED,
    REQUEST_PROCESSING_ERROR,
    REQUEST_SERVICE_ERROR,
    REQUEST_RECIEVE_FAILED,
    REQUEST_TIMEOUT,
    REQUEST_RECV_FAILED,
    REQUEST_CONNECTION_CLOSED
} RequestStatus;

class RequestHandler
{
private:
    int handledRequests = 0;

    const SOCKET client_socket_fh;
    const sockaddr_in server_addr;
    const std::string ipString;

    const std::function<int(Request &req, Response &res)> &service;

    std::stringstream requestHeaderStream;
    std::stringstream requestBodyStream;
    std::vector<unsigned char> requestBodyBuffer;
    std::unordered_map<std::string, std::string> requestHeadersMap;

    std::vector<unsigned char> responseBuffer;

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
        @brief: this is the main request-response handling loop
        @note: invokes 'service' function passed from HttpServer
    */
    int handleReqRes();

    /*
        Parsed request header is stored in the 'requestHeadersMap' and body in 'requstbodyBuffer' (Fixed it)
        @returns: 'Content-Length' from the header-field, on error returns -1 casted to size_t
    */
    int parseRequestHeaders();

    bool isConnectionKeepAlive() const;

    int startReceiving();

    bool startSending() const;

    bool clearOneReqResCycle();

    std::string assignIpStr() const;
};
