#include "../include/Logger.hpp"
#include "../include/HttpServer.hpp"
#include "../include/RequestHandler.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/HttpResponse.hpp"

HttpServer::HttpServer() : port(PORT) {}

HttpServer::HttpServer(short port) : port(port) {}

void HttpServer::init()
{
    if (this->initTCPconnection() < 0)
        Logger::err("Couldn't Initiate TCP connecton");
    else
        Logger::status("TCP connection made");
}

int HttpServer::initTCPconnection()
{
    WSA_versionReq = MAKEWORD(2, 2);
    if (WSAStartup(WSA_versionReq, &wsaData) < 0)
    {
        Logger::err("WSA Startup failed");
        WSACleanup();
        return -1;
    }
    Logger::status("Wsa StartUp successful");

    if ((this->server_socket_fh = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Logger::err("Socket creation failed", this->server_socket_fh);
        WSACleanup();
        return -1;
    }
    Logger::status("Socket created");

    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(port);
    this->server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);

    if (bind(this->server_socket_fh, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) < 0)
    {
        Logger::err("Binding failed", this->server_socket_fh);
        WSACleanup();
        return -1;
    }
    Logger::status("Binding successful");

    if (listen(this->server_socket_fh, 50) < 0)
    {
        Logger::err("Listening failed", this->server_socket_fh);
        WSACleanup();
        return -1;
    }
    Logger::status("Server is listening to Port '" + std::to_string(this->port) + "'");

    // acceptor
    std::thread acceptorWorker(HttpServer::requestAcceptor, this);
    acceptorWorker.join();
    return 0;
}

int HttpServer::requestAcceptor()
{
    try
    {
        while (true)
        {
            SOCKET client_socket_fh;
            server_addr_len = sizeof(this->server_addr);
            if ((client_socket_fh = accept(this->server_socket_fh, (sockaddr *)&this->server_addr, &server_addr_len)) < 0)
            {
                Logger::err("Couldn't accept request INVALID SOCKET");
            }
            else
            {
                size_t addressSize = sizeof(this->server_addr.sin_addr.S_un.S_addr);
                char bytes[addressSize];
                strncpy(bytes, (char *)&this->server_addr.sin_addr.S_un.S_addr, addressSize);

                std::string ipString;
                for (int i = 0; i < 4; i++)
                {
                    ipString += std::to_string(bytes[i]);
                    if (i != 4 - 1)
                        ipString += ".";
                }

                Logger::logs("User connected with IP: " + ipString + " and PORT: " + std::to_string(this->server_addr.sin_port));

                std::thread reqHandleWorker(HttpServer::reqInstantiator, this, client_socket_fh, server_addr);
                reqHandleWorker.detach();
            }
        }
    }
    catch (const std::exception &e)
    {
        Logger::err("Exception in initTCPconnection: " + std::string(e.what()));
        WSACleanup();
        return -1;
    }
    return 0;
}

int HttpServer::reqInstantiator(SOCKET client_socket_fh, sockaddr_in server_addr)
{
    RequestHandler *client = new RequestHandler(
        client_socket_fh,
        server_addr,
        std::bind(&HttpServer::service,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2)

    );

    delete client;
    return 0;
}

int HttpServer::service(Request &req, Response &res)
{
    std::string method = req.getMethod();

    middleWare(req, res);

    int servicesStatus;
    if (method == "GET")
        servicesStatus = serveGET(req, res);
    else if (method == "POST")
        servicesStatus = servePOST(req, res);
    else if (method == "PUT")
        servicesStatus = servePUT(req, res);
    else if (method == "PATCH")
        servicesStatus = servePATCH(req, res);
    else if (method == "DELETE")
        servicesStatus = serveDELETE(req, res);
    else
        servicesStatus = serveSPECIFIC(req, res);
    return servicesStatus;
}

int HttpServer::middleWare(Request &req, Response &res)
{
    return 0;
}

// these methods will be overrided by the inheriting class, so kept blank
// might add something if required...
int HttpServer::serveGET(Request &req, Response &res)
{
    return 0;
}

int HttpServer::servePOST(Request &req, Response &res)
{
    return 0;
}

int HttpServer::servePUT(Request &req, Response &res)
{
    return 0;
}

int HttpServer::servePATCH(Request &req, Response &res)
{
    return 0;
}

int HttpServer::serveDELETE(Request &req, Response &res)
{
    return 0;
}

int HttpServer::serveSPECIFIC(Request &req, Response &res)
{
    return 0;
}