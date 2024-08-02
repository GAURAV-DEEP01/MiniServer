#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "AppIncludes.hpp"

class HttpServer
{
private:
    short port;
    WORD WSA_versionReq;
    WSADATA wsaData;

    struct sockaddr_in server_addr;

public:
    // initializing server
    HttpServer();
    HttpServer(short Port);

private:
    int initTCPconnection();
};

#endif