#include "../include/RequestHandler.hpp"
#include "../include/Logger.hpp"

RequestHandler::RequestHandler(
    SOCKET client_socket_fh,
    sockaddr_in server_addr,
    const std::function<int(Request &req, Response &res)> &service)
    : requestHeadersMap(),
      client_socket_fh(client_socket_fh),
      service(service),
      server_addr(server_addr),
      isHandlerActive(false)
{
    int timeout = 5000;
    setsockopt(client_socket_fh, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

    size_t addressSize = sizeof(this->server_addr.sin_addr.S_un.S_addr);
    char bytes[addressSize];
    strncpy(bytes, (char *)&this->server_addr.sin_addr.S_un.S_addr, addressSize);

    for (int i = 0; i < 4; i++)
    {
        ipString += std::to_string(bytes[i]);
        if (i != 4 - 1)
            ipString += ".";
    }

    handleReqRes();
}

RequestHandler::~RequestHandler()
{
    Logger::logs("Connection closed, IP: " +
                 ipString +
                 " and PORT: " +
                 std::to_string(this->server_addr.sin_port));
    shutdown(client_socket_fh, SD_BOTH);
    closesocket(client_socket_fh);
}

int RequestHandler::handleReqRes()
{
    // std::thread inactiveMonitor(RequestHandler::monitorClientInactivity, this);
    while (maxRequest > handledRequests++)
    {
        if (!startReciving())
            break;

        // stack allocation for now while testing... i dont want things to break in this stage
        Request req(requestHeadersMap, requestBodyStream);
        Response res(responseStream);

        Logger::info(requestHeaderStream.str());

        // request response handle will be done here...
        if (service(req, res) < 0)
            break;

        res.startWriter();

        Logger::info(responseStream.str());

        if (!startSending())
            break;

        if (!clearOneReqResCycle())
            break;
    }
    // inactiveMonitor.join();
    return 0;
}

bool RequestHandler::isConnectionKeepAlive()
{
    std::string connectionKey = "Connection";
    auto connection = requestHeadersMap.find(connectionKey);

    if (connection != requestHeadersMap.end() && connection->second == "keep-alive")
        return true;
    Logger::info(connectionKey + "=" + (connection != requestHeadersMap.end() ? connection->second : "not found"));
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
            return false;
        }
        else if (bytesRead == INVALID_SOCKET)
        {
            Logger::logs("Connection failed! or Timeout, IP: " + ipString +
                         " and PORT: " +
                         std::to_string(this->server_addr.sin_port));
            return false;
        }
        isHandlerActive.store(true);
        char *headerEndChar = strstr(readBuffer, "\r\n\r\n");

        if (isReadingHeader && headerEndChar != nullptr)
        {
            isReadingHeader = false;
            *headerEndChar = '\0';

            *currentRequestStream << readBuffer;
            currentRequestStream = &requestBodyStream;

            char *reqBodyStartChar = headerEndChar + 4;
            *currentRequestStream << reqBodyStartChar;

            if ((contentLength = requestParserHeader()) == -1)
                return false;

            int diff = headerEndChar - readBuffer;
            contentLength -= bytesRead - (diff + 4);
            if (contentLength == 0)
                break;
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
            Logger::err("Send failed," +
                            ipString +
                            " and PORT: " +
                            std::to_string(this->server_addr.sin_port) +
                            std::string(strerror(WSAGetLastError())),
                        client_socket_fh);
            return false;
        }
    }
    Logger::logs("Send complete, IP: " +
                 ipString +
                 " and PORT: " +
                 std::to_string(this->server_addr.sin_port));
    return true;
}

static std::string trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
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
        std::getline(requestHeaderStream, value);
        key = trim(key);
        value = trim(value);
        if (!key.empty() && !value.empty())
            requestHeadersMap[key] = value;
    }
    if (!requestHeaderStream.eof())
    {
        Logger::err("Couldn't parse header");
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
    if (!isConnectionKeepAlive())
        return false;
    requestHeaderStream.clear();
    requestBodyStream.clear();
    responseStream.clear();
    requestHeadersMap.clear();
    isHandlerActive.store(true);
    handledRequests++;
    return true;
}

void RequestHandler::monitorClientInactivity()
{
    const int monitoringInterval = 5;
    while (isHandlerActive.load())
        std::this_thread::sleep_for(std::chrono::seconds(monitoringInterval));
    shutdown(client_socket_fh, SD_BOTH);
}