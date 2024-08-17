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
    setsockopt(client_socket_fh, SOL_SOCKET, SO_RCVTIMEO,
               (const char *)&maxTimout,
               sizeof(maxTimout));

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
    while (maxRequest > handledRequests++)
    {
        if (!startReciving())
            break;

        std::unique_ptr<Request> req = std::make_unique<Request>(requestHeadersMap, requestBodyStream);
        std::unique_ptr<Response> res = std::make_unique<Response>(responseBuffer);

        // request response handle will be done here...
        if (service(*req, *res) < 0)
            break;

        res->startWriter();

        if (!startSending())
            break;

        if (!clearOneReqResCycle())
            break;
    }
    return 0;
}

bool RequestHandler::isConnectionKeepAlive()
{
    std::string connectionKey = "Connection";
    auto connection = requestHeadersMap.find(connectionKey);

    if (connection != requestHeadersMap.end() && connection->second == "keep-alive")
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
            return false;
        }
        else if (bytesRead == INVALID_SOCKET)
        {
            Logger::logs("Connection Timeout or failed!, IP: " + ipString +
                         " and PORT: " +
                         std::to_string(this->server_addr.sin_port));
            return false;
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

            if ((contentLength = requestParserHeader()) == (size_t)-1)
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
    int bytesSent = send(client_socket_fh, (char *)responseBuffer.data(), responseBuffer.size(), 0);
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
    requestHeadersMap["Version"] = value;

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
    requestHeaderStream.str("");

    requestBodyStream.clear();
    requestBodyStream.str("");

    responseBuffer.clear();
    requestHeadersMap.clear();

    handledRequests++;
    return true;
}
