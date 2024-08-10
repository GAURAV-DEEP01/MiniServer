#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "AppIncludes.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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

    /*
        calls the initTCPconnection method
        Invoke this method after object instantiation to start the server listening to port
        Note: this function will pause the caller thread
    */
    void init();

    virtual int service(Request &req, Response &res);

    // self discriptive HTTP methods
    virtual int serveGET(Request &req, Response &res);
    virtual int servePOST(Request &req, Response &res);
    virtual int servePUT(Request &req, Response &res);
    virtual int servePATCH(Request &req, Response &res);
    virtual int serveDELETE(Request &req, Response &res);

    /*
        this function is used for other additonsl methods except the default HTTP methods above
        if you override this method use 'getMethod()' [In Request object] function to get the specific method sent by the client
    */
    virtual int serveSPECIFIC(Request &req, Response &res);

    // will implimente this in the next commit (might reconsider)
    void defaultService(Request &req, Response &res);

private:
    /*
        initialises TCP connection and listens to a default or given port
        Note: Join thread is called here so this will pause the main thread (Might remove the thread if not nessessary)
    */
    int initTCPconnection();

    /*
        this method runs in a separate thread, continuously accepting incoming requests from clients.
        for each accepted request, it creates a new thread that invokes the 'reqInstantiator()' with the client socket.
    */
    int requestAcceptor();

    /*
        creates 'RequestHandler' object with the client socket in
        the heap and clears the allcoated memory after request has been serviced
    */
    int reqInstantiator(SOCKET client_socket_fh);
};

#endif