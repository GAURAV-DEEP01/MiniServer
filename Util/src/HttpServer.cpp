#include "../include/Logger.hpp"
#include "../include/HttpServer.hpp"
#include "../include/RequestHandler.hpp"

HttpServer::HttpServer() : port(PORT)
{
    if (this->initTCPconnection() < 0)
        Logger::err("Couldn't Initiate TCP connecton");
    else
        Logger::status("TCP connection made");
}

HttpServer::HttpServer(short port) : port(port)
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
        return -1;
    }
    Logger::status("Wsa StartUp successfull");

    this->server_socket_fh;
    if ((this->server_socket_fh = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Logger::err("Socket creation failed", this->server_socket_fh);
        return -1;
    }
    Logger::status("Socket created");

    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(port);
    this->server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);

    if (bind(this->server_socket_fh, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) < 0)
    {
        Logger::err("Binding failed", this->server_socket_fh);
        return -1;
    }
    Logger::status("Binding successfull");

    if (listen(this->server_socket_fh, 10) < 0)
    {
        Logger::err("Listening failed", this->server_socket_fh);
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
    while (1)
    {
        SOCKET client_socket_fh;
        server_addr_len = sizeof(this->server_addr);
        if ((client_socket_fh = accept(this->server_socket_fh, (sockaddr *)&this->server_addr, &server_addr_len)) < 0)
        {
            Logger::err("Couldn't accept request");
        }
        else
        {
            // req res
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

            std::thread reqHandleWorker(HttpServer::reqInstantiator, this, client_socket_fh);
            reqHandleWorker.detach();
        }
    }
    return 0;
}

int HttpServer::reqInstantiator(SOCKET client_socket_fh)
{
    RequestHandler *client = new RequestHandler(client_socket_fh);

    delete client;
    return 0;
}