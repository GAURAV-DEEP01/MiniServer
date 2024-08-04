#include "../include/RequestHandler.hpp"
#include "../include/Logger.hpp"

RequestHandler::RequestHandler(SOCKET client_socket_fh) : requestHeadersMap(), client_socket_fh(client_socket_fh)
{
    handleReqRes();
}

void RequestHandler::handleReqRes()
{
    bool isReadingHeader = true;
    std::stringstream *currentRequestStream = &requestHeaderStream;
    char requestBuffer[100];
    while (1)
    {
        memset(requestBuffer, 0, sizeof(requestBuffer));
        size_t bytesRead;
        if ((bytesRead = recv(client_socket_fh, requestBuffer, sizeof(requestBuffer) - 1, 0)) == 0)
        {
            Logger::logs("connection closed");
            return;
        }
        char *headerEndChar = strstr(requestBuffer, "\r\n\r\n");

        if (headerEndChar != nullptr && isReadingHeader)
        {
            isReadingHeader = false;
            *headerEndChar = '\0';
            *currentRequestStream << requestBuffer;
            currentRequestStream = &requestBodyStream;

            // parsing to get content-length it req has it
            this->requestParserHeader();
            char *reqBodyStartChar = headerEndChar + 4;
            *currentRequestStream << reqBodyStartChar;
        }
        else
        {
            *currentRequestStream << requestBuffer;
        }
    }

    // damn parsing
    const char response[300] = "HTTP/1.1 200 OK\r\nDate: Fri, 02 Aug 2024 10:00:00 GMT\r\nServer: GDHTTPServer/1.0\r\nContent-Length: 11\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n<h1>hi</h1>\r\n";

    int bytesSent = send(client_socket_fh, response, 300, 0);
    if (bytesSent == SOCKET_ERROR)
    {
        Logger::err("Send failed: " + std::string(strerror(WSAGetLastError())));
        return;
    }
    closesocket(client_socket_fh);

    Logger::logs("Send complete");
}

int RequestHandler::requestParserHeader()
{
    std::string key;
    std::string value;

    Logger::info(requestHeaderStream.str());
    while (std::getline(requestHeaderStream, key, ':'))
    {
        requestHeaderStream.ignore(1);
        std::getline(requestHeaderStream, value);
        requestHeadersMap[key] = value;
    }
    if (!requestHeaderStream.eof())
        Logger::status("couldn't parse header");

    auto it = requestHeadersMap.find("Content-length");
    if (it != requestHeadersMap.end())
    {
        return std::stoi(it->second);
    }
    return 0;
}
