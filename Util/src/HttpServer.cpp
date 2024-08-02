#include "../include/Logger.hpp"
#include "../include/HttpServer.hpp"

HttpServer::HttpServer() : port(PORT)
{
    if (this->initTCPconnection() < 0)
        Logger::err("Could'nt Initiate TCP connecton");
    else
        Logger::status("TCP connection made");
}

HttpServer::HttpServer(short port) : port(port)
{
    if (this->initTCPconnection() < 0)
        Logger::err("Could'nt Initiate TCP connecton");
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

    SOCKET socket_fh;
    if ((socket_fh = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Logger::err("Socket creation failed", socket_fh);
        return -1;
    }
    Logger::status("Socket created");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);

    if (bind(socket_fh, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        Logger::err("Binding failed", socket_fh);
        return -1;
    }
    Logger::status("Binding successfull");

    if (listen(socket_fh, 10) < 0)
    {
        Logger::err("Listening failed", socket_fh);
        return -1;
    }
    Logger::status("Server is listening to Port '" + std::to_string(port) + "'");

    SOCKET client_socket_fh;
    int server_addr_len = sizeof(server_addr);
    if ((client_socket_fh = accept(socket_fh, (sockaddr *)&server_addr, &server_addr_len)) < 0)
    {
        Logger::err("Could'nt accept request");
        return -1;
    }

    size_t addressSize = sizeof(server_addr.sin_addr.S_un.S_addr);
    char bytes[addressSize];
    strncpy(bytes, (char *)&server_addr.sin_addr.S_un.S_addr, addressSize);

    std::string ipString;
    for (int i = 0; i < 4; i++)
    {
        ipString += std::to_string(bytes[i]);
        if (i != 4 - 1)
            ipString += ":";
    }

    Logger::logs("User connected with ip " + ipString + " and port" + std::to_string(server_addr.sin_port));

    char buffer[1000];
    size_t bytesRead;
    if ((bytesRead = recv(client_socket_fh, buffer, sizeof(buffer), 0)) == 0)
        Logger::logs("connection closed");
    break;
    else Logger::info(std::string(buffer));

    const char response[300] = "HTTP/1.1 200 OK\r\nDate: Fri, 02 Aug 2024 10:00:00 GMT\r\nServer: GDHTTPServer/1.0\r\nContent-Length: 11\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n<h1>hi</h1>\r\n";

    int bytesSent = send(client_socket_fh, response, 300, 0);
    if (bytesSent == SOCKET_ERROR)
    {
        Logger::err("Send failed: " + std::string(strerror(WSAGetLastError())));
        return -1;
    }
    else
    {
        Logger::info("Sent " + std::to_string(bytesSent) + " bytes");
    }
    closesocket(client_socket_fh);

    Logger::logs("Send complete");

    return 0;
}
