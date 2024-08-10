#include "../include/RequestHandler.hpp"
#include "../include/Logger.hpp"

RequestHandler::RequestHandler(
    SOCKET client_socket_fh,
    const std::function<int(Request &req, Response &res)> &service)
    : requestHeadersMap(),
      client_socket_fh(client_socket_fh),
      service(service)
{
    handleReqRes();
}

int RequestHandler::handleReqRes()
{
    while (maxRequest > handledRequests)
    {
        if (!startReciving())
            return 0;

        INFO(requestBodyStream.str());

        // stack allocation for now while testing... i dont want things to break in this stage
        Request req(requestHeadersMap, requestBodyStream);
        Response res(responseStream);

        // request response handle will be done here...
        if (service(req, res) < 0)
        {
            closesocket(client_socket_fh);
            return 0;
        }

        res.startWriter();

        if (!startSending())
            return 0;
        if (!clearOneReqResCycle())
            return 0;
    }
    closesocket(client_socket_fh);
    return 0;
}

bool RequestHandler::isConnectionKeepAlive()
{
    auto connection = requestHeadersMap.find("Connection");
    if ((connection != requestHeadersMap.end()) && connection->second == "keep-alive")
        return true;
    return false;
}

bool RequestHandler::startReciving()
{
    bool isReadingHeader = true;
    std::stringstream *currentRequestStream = &requestHeaderStream;
    char readBuffer[100];
    size_t contentLength = INFINITE;
    while (true)
    {
        memset(readBuffer, 0, sizeof(readBuffer));
        size_t bytesRead;
        if ((bytesRead = recv(client_socket_fh, readBuffer, sizeof(readBuffer) - 1, 0)) == 0)
        {
            Logger::logs("Connection closed");
            return true;
        }
        isHandlerActive = true;
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
                if ((contentLength = this->requestParserHeader()) < 0)
                    return false;
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
    return true;
}

bool RequestHandler::startSending()
{
    char writeBuffer[300];
    while (responseStream.read(writeBuffer, sizeof(writeBuffer) - 1) || responseStream.gcount() > 0)
    {
        writeBuffer[responseStream.gcount()] = '\0';
        int bytesSent = send(client_socket_fh, writeBuffer, sizeof(writeBuffer), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            Logger::err("Send failed: " + std::string(strerror(WSAGetLastError())), client_socket_fh);
            return false;
        }
    }
    Logger::logs("Send complete");
    return true;
}

int RequestHandler::requestParserHeader()
{
    std::string key;
    std::string value;

    // request line parsing
    requestHeaderStream >> value;
    requestHeadersMap["Method"] = value;

    requestHeaderStream >> value;
    requestHeadersMap["Url"] = value;

    requestHeaderStream >> value;
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

bool RequestHandler::clearOneReqResCycle()
{
    requestHeaderStream.clear();
    requestHeadersMap.clear();
    requestBodyStream.clear();
    responseStream.clear();
    if (!isConnectionKeepAlive())
        return false;

    handledRequests++;
    return true;
}