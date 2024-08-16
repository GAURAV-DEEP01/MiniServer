#include "../include/Logger.hpp"
#include "../include/HttpServer.hpp"
#include "../include/RequestHandler.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/HttpResponse.hpp"

HttpServer::HttpServer() : port(PORT) {}

void HttpServer::listen(short port)
{
    middleWare = [](Request &req, Response &res) -> int
    {
        return 0;
    };
    this->port = port;
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

    if ((this->server_socket_fh = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
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

    if (::listen(this->server_socket_fh, 50) < 0)
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
            if ((client_socket_fh = accept(this->server_socket_fh, (sockaddr *)&this->server_addr, &server_addr_len)) == INVALID_SOCKET)
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

    if (servicesStatus == METHOD_NOT_FOUND)
        defaultService(req, res);
    return servicesStatus;
}

int HttpServer::route(
    Request &req, Response &res,
    std::unordered_map<std::string, std::function<int(Request &, Response &)>> &route)
{
    auto foundMethod = route.find(req.getBaseRouteUrl());
    if (foundMethod != route.end())
        return foundMethod->second(req, res);
    return METHOD_NOT_FOUND;
}

// redesigned
int HttpServer::serveGET(Request &req, Response &res)
{
    return route(req, res, routeGet);
}
int HttpServer::servePOST(Request &req, Response &res)
{
    return route(req, res, routePost);
}

int HttpServer::servePUT(Request &req, Response &res)
{
    return route(req, res, routePut);
}

int HttpServer::servePATCH(Request &req, Response &res)
{
    return route(req, res, routePatch);
}

int HttpServer::serveDELETE(Request &req, Response &res)
{
    return route(req, res, routeDelete);
}

int HttpServer::serveSPECIFIC(Request &req, Response &res)
{
    return route(req, res, routeSpecific);
}

// override this method to route NOT FOUND pages
void HttpServer::defaultService(Request &req, Response &res)
{
    res.setStatus(404);
    res.setReasonPhrase("NOT FOUND");
    res.setContentType("text/plain");
    res.writeToBody("Host: " + req.getHost() + "\n");
    res.writeToBody("URL: " + req.getUrl() + "\n");
    res.writeToBody("METHOD: " + req.getMethod() + "\n");
    res.writeToBody("Error: Route Not Found");
}