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
    // initializing server with defauld port 23000
    HttpServer();

    // initializing server with userdefined port
    HttpServer(short Port);

    // calls the initTCPconnection method
    void init();

    virtual int service();

private:
    /*
        initialises TCP connection and listens to a default or given port
        Note: Join thread is called here so this will pause the main thread (Might remove the thread if not nessessary)
    */
    int initTCPconnection();

    /*
        this is a threaded method constently accepting incomming requests from the client
        for each reqest accepted this creates thread invoking the reqInstantiator with the client socket
    */
    int requestAcceptor();

    /*
        creates 'RequestHandler' object with the client socket in
        the heap and clears the allcoated memory after request has been serviced
    */
    int reqInstantiator(SOCKET client_socket_fh);
};

#endif