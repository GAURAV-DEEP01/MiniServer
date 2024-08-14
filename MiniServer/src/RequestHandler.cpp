#include "../include/RequestHandler.hpp"
#include "../include/Logger.hpp"

RequestHandler::RequestHandler(
    SOCKET client_socket_fh,
    sockaddr_in server_addr,
    const std::function<int(Request &req, Response &res)> &service)
    : requestHeadersMap(),
      client_socket_fh(client_socket_fh),
      service(service),
      server_addr(server_addr)
{
    handleReqRes();
}

RequestHandler::~RequestHandler()
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

    Logger::logs("Connection closed for IP: " +
                 ipString +
                 " and PORT: " +
                 std::to_string(this->server_addr.sin_port));
    closesocket(client_socket_fh);
}

int RequestHandler::handleReqRes()
{
    std::thread inactiveMonitor(RequestHandler::monitorClientInactivity, this);
    while (maxRequest > handledRequests++)
    {
        if (!startReciving())
            break;

        // stack allocation for now while testing... i dont want things to break in this stage
        Request req(requestHeadersMap, requestBodyStream);
        Response res(responseStream);

        INFO(requestHeaderStream.str());
        // request response handle will be done here...
        if (service(req, res) < 0)
            break;

        res.startWriter();

        INFO(responseStream.str());

        if (!startSending())
            break;
        if (!clearOneReqResCycle())
            break;
    }
    inactiveMonitor.join();
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
            return false;
        else if (bytesRead < 0)
        {
            Logger::logs("Connection failed!");
            return false;
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
    while (responseStream.read(writeBuffer, sizeof(writeBuffer) - 1) ||
           responseStream.gcount() > 0)
    {
        writeBuffer[responseStream.gcount()] = '\0';
        int bytesSent = send(client_socket_fh, writeBuffer, sizeof(writeBuffer), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            Logger::err("Send failed: " + std::string(strerror(WSAGetLastError())),
                        client_socket_fh);
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
    {
        Logger::info("false");
        return false;
    }
    else
        Logger::info("true");
    isHandlerActive = false;
    handledRequests++;
    return true;
}

void RequestHandler::monitorClientInactivity()
{

    const int monitoringInterval = 2;
    std::this_thread::sleep_for(std::chrono::seconds(monitoringInterval));
    shutdown(client_socket_fh, SD_BOTH);
    Logger::info("After 10 seconds! now the socket is closed!");
}