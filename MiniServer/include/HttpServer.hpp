#pragma once

#include "AppIncludes.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

typedef enum
{
    SERVER_ERROR = -1,
    SERVER_SAFE_STATE,
    SERVER_ROUT_NOT_FOUND,
    SERVER_MIDDLEWARE_ERROR
} ServerStatus;

class HttpServer
{
protected:
    short port;
    WORD WSA_versionReq;
    WSADATA wsaData;
    SOCKET server_socket_fh;
    struct sockaddr_in server_addr;
    int server_addr_len;

    /*
        invoke's the 'middleWare' method before the serve methods
        invoke's the serve methods serveGET, servePOST, servePUT, servePATCH, serveDELETE or
        serveSPECIFIC (for HTTP methods except the above mentioned) based on the clients request HTTP method
    */
    virtual int service(Request &req, Response &res);

    // Routes the client HTTP methods to respective umap funtion
    virtual int serveGET(Request &req, Response &res);
    virtual int servePOST(Request &req, Response &res);
    virtual int servePUT(Request &req, Response &res);
    virtual int servePATCH(Request &req, Response &res);
    virtual int serveDELETE(Request &req, Response &res);

    /*
        Used for other additonsl methods except the default HTTP methods above
        if you override this method use 'getMethod()' [In Request object] function to get the specific method sent by the client
    */
    virtual int serveSPECIFIC(Request &req, Response &res);

    // override this method if you want to serve not found response
    virtual int defaultService(Request &req, Response &res);

    std::unordered_map<std::string, std::function<int(Request &, Response &)>> routeGet;
    std::unordered_map<std::string, std::function<int(Request &, Response &)>> routePost;
    std::unordered_map<std::string, std::function<int(Request &, Response &)>> routePut;
    std::unordered_map<std::string, std::function<int(Request &, Response &)>> routePatch;
    std::unordered_map<std::string, std::function<int(Request &, Response &)>> routeDelete;
    std::unordered_map<std::string, std::function<int(Request &, Response &)>> routeSpecific;

public:
    // Can be use to maintain/limit number of threads for the Server 
    std::atomic<int> currentThreadCount;
    std::atomic<int> clientsServed;

    static const int maxRequest = 100;
    static const int maxTimeout = 15000;


    HttpServer();

    /*
        invoke's the initTCPconnection method
        Invoke's this method after object instantiation to start the server listening to port
        Note: this function will pause the caller thread
    */
    void listen(short port);

    /*
        this method is invoked before any of the route methods, so general request pre-processing can be done
        for route specific middleware there is no current solution except for pre-processing request at the start of each route method
    */
    std::function<int(Request &req, Response &res)> middleWare;

    /*
        @brief: 
        In the "handler" method, return 0 or 'SERVER_SAFE_STATE' if no errors
        return -1 or SERVER_ERROR to close the connection and discard the request

        @returns: false if route already exists, true if it does not
    */

    bool Get(const std::string &path, const std::function<int(Request &, Response &)> &handler);
    bool Put(const std::string &path, const std::function<int(Request &, Response &)> &handler);
    bool Post(const std::string &path, const std::function<int(Request &, Response &)> &handler);
    bool Patch(const std::string &path, const std::function<int(Request &, Response &)> &handler);
    bool Delete(const std::string &path, const std::function<int(Request &, Response &)> &handler);
    bool Specific(const std::string &path, const std::function<int(Request &, Response &)> &handler);

private:
    /*
        initialises TCP connection and listens to a default or given port
        Note: Join thread is called here so this will pause the main thread (Might remove the thread if not nessessary)
    */
    int initTCPconnection();

    /*
        this runs in a separate thread, continuously accepting incoming requests from clients.
        for each accepted request, it creates a new thread that invokes the 'reqInstantiator()' with the client socket.
    */
    int requestAcceptor();

    /*
        creates 'RequestHandler' object with the client socket in
        the heap and clears the allcoated memory after request has been serviced
    */
    int reqInstantiator(SOCKET client_socket_fh, sockaddr_in server_addr);

    int route(Request &req, Response &res, std::unordered_map<std::string, std::function<int(Request &, Response &)>> &handler);

    bool checkValidity(const std::string &path, const std::unordered_map<std::string, std::function<int(Request &, Response &)>> &handler) const;
};
