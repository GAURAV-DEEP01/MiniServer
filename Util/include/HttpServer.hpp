#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "AppIncludes.hpp"

class HttpServer
{
protected:
    short port;
    WORD WSA_versionReq;
    WSADATA wsaData;
    SOCKET server_socket_fh;
    struct sockaddr_in server_addr;
    int server_addr_len;

public:
    // initializing server
    HttpServer();
    HttpServer(short Port);
    void init();
    virtual int service();

private:
    int initTCPconnection();
    int requestAcceptor();
    int reqInstantiator(SOCKET client_socket_fh);
};

#endif