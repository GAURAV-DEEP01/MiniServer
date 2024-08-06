#include "../include/RequestHandler.hpp"
#include "../include/Logger.hpp"

RequestHandler::RequestHandler(SOCKET client_socket_fh, const std::function<int(Request &req, Response &res)> &service)
    : requestHeadersMap(), client_socket_fh(client_socket_fh), service(service)
{
    handleReqRes();
}

int RequestHandler::handleReqRes()
{
    bool isReadingHeader = true;
    std::stringstream *currentRequestStream = &requestHeaderStream;
    char readBuffer[100];
    size_t contentLength = INFINITE;
    while (1)
    {
        memset(readBuffer, 0, sizeof(readBuffer));
        size_t bytesRead;
        if ((bytesRead = recv(client_socket_fh, readBuffer, sizeof(readBuffer) - 1, 0)) == 0)
        {
            Logger::logs("connection closed");
            return 0;
        }
        char *headerEndChar = strstr(readBuffer, "\r\n\r\n");

        if (isReadingHeader && headerEndChar != nullptr)
        {
            isReadingHeader = false;
            *headerEndChar = '\0';
            *currentRequestStream << readBuffer;
            currentRequestStream = &requestBodyStream;

            char *reqBodyStartChar = headerEndChar + 4;
            *currentRequestStream << reqBodyStartChar;
            if (contentLength == 0)
                break;
            else
            {
                contentLength = this->requestParserHeader();
                int diff = (headerEndChar - readBuffer);
                contentLength -= bytesRead - (diff + 4);
                if (contentLength == 0)
                    break;
            }
        }
        else
        {
            *currentRequestStream << readBuffer;
            if (!isReadingHeader)
                contentLength -= bytesRead;
            if (contentLength <= 0)
                break;
        }
    }
    INFO(requestBodyStream.str());

    // request response handle will be done here...
    service(req, res);

    const char response[300] = "HTTP/1.1 200 OK\r\nDate: Fri, 02 Aug 2024 10:00:00 GMT\r\nServer: GDHTTPServer/1.0\r\nContent-Length: 11\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n<h1>hi</h1>\r\n";

    int bytesSent = send(client_socket_fh, response, 300, 0);
    if (bytesSent == SOCKET_ERROR)
    {
        Logger::err("Send failed: " + std::string(strerror(WSAGetLastError())));
        return -1;
    }
    closesocket(client_socket_fh);

    Logger::logs("Send complete");
    return 0;
}

int RequestHandler::requestParserHeader()
{
    std::string key;
    std::string value;

    // request line parsing
    std::getline(requestHeaderStream, value, ' ');
    requestHeadersMap["Method"] = value;
    std::getline(requestHeaderStream, value, ' ');
    requestHeadersMap["Url"] = value;
    std::getline(requestHeaderStream, value);
    requestHeadersMap["version"] = value;

    // header field parsing
    while (std::getline(requestHeaderStream, key, ':'))
    {
        requestHeaderStream.ignore(1);
        std::getline(requestHeaderStream, value);
        requestHeadersMap[key] = value;
    }
    if (!requestHeaderStream.eof())
    {
        Logger::status("Couldn't parse header");
        return -1;
    }
    INFO(requestHeaderStream.str());
    auto bodySize = requestHeadersMap.find("Content-Length");
    if (bodySize != requestHeadersMap.end())
    {
        return std::stoi(bodySize->second);
    }
    return 0;
}
