#include "../include/RequestHandler.hpp"
#include "../include/Logger.hpp"

RequestHandler::RequestHandler(
    SOCKET client_socket_fh,
    sockaddr_in server_addr,
    const std::function<int(Request &req, Response &res)> &service)
    : client_socket_fh(client_socket_fh),
      server_addr(server_addr),
      ipString(assignIpStr()),
      service(service),
      requestHeadersMap()
{
    setsockopt(client_socket_fh, SOL_SOCKET, SO_RCVTIMEO,
               (const char *)&maxTimout,
               sizeof(maxTimout));

    std::string ipPort = "IP: " + ipString + " and PORT: " + std::to_string(server_addr.sin_port);

    const int handleReqResStatus = handleReqRes();
    switch (handleReqResStatus)
    {
    case REQUEST_PROCESSING_ERROR:
        Logger::logs("Request handle failed," + ipPort);
        break;
    case REQUEST_LIMIT_EXCEEDED:
        Logger::logs("Request's-per-connection limit  exceeded, " + ipPort);
        break;
    case REQUEST_SERVICE_ERROR:
        Logger::logs("Request service failed, " + ipPort);
        break;
    case REQUEST_RECV_FAILED:
        Logger::logs("Request 'recv()' failed, " + ipPort);
        break;
    }
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
        const int recevingStatus = startReceiving();
        if (recevingStatus == REQUEST_RECIEVE_FAILED)
            return REQUEST_PROCESSING_ERROR;
        if (recevingStatus == REQUEST_CONNECTION_CLOSED)
            return REQUEST_SAFE_STATE;
        if (recevingStatus == REQUEST_RECV_FAILED)
            return recevingStatus;

        std::unique_ptr<Request> req = std::make_unique<Request>(requestHeadersMap, requestBodyBuffer);
        std::unique_ptr<Response> res = std::make_unique<Response>(responseBuffer);

        if (service(*req, *res) < 0)
            return REQUEST_SERVICE_ERROR;

        if (!res->_startWriter())
            return REQUEST_PROCESSING_ERROR;

        if (!startSending())
            return REQUEST_PROCESSING_ERROR;

        if (!clearOneReqResCycle())
            return REQUEST_SAFE_STATE;
    }
    return REQUEST_LIMIT_EXCEEDED;
}

bool RequestHandler::isConnectionKeepAlive() const
{
    std::string connectionKey = "Connection";
    const auto connection = requestHeadersMap.find(connectionKey);

    if ((connection != requestHeadersMap.end()) && (connection->second == "keep-alive"))
        return true;

    return false;
}

int RequestHandler::startReceiving()
{
    bool isReadingHeader = true;
    char readBuffer[2048];
    size_t contentLength = INFINITE;
    while (true)
    {
        memset(readBuffer, 0, sizeof(readBuffer));
        size_t bytesRead;
        if ((bytesRead = recv(client_socket_fh, readBuffer, sizeof(readBuffer) - 1, 0)) == 0)
        {
            return REQUEST_CONNECTION_CLOSED;
        }
        if (bytesRead == INVALID_SOCKET)
        {
            bool isTimeout;
            std::string statusMsg = ((isTimeout = (WSAGetLastError()) == WSAETIMEDOUT) ? true : false) ? "Timeout" : "Failed";
            Logger::logs("Connection " + statusMsg + "!, IP: " + ipString +
                         " and PORT: " +
                         std::to_string(this->server_addr.sin_port));

            return isTimeout ? REQUEST_TIMEOUT : REQUEST_RECV_FAILED;
        }
        char *headerEndChar = strstr(readBuffer, "\r\n\r\n");

        if (isReadingHeader && headerEndChar != nullptr)
        {
            isReadingHeader = false;
            *headerEndChar = '\0';

            requestHeaderStream << readBuffer;

            const char *reqBodyStartChar = headerEndChar + 4;
            const size_t bodyBufferSize = bytesRead - ((reqBodyStartChar - readBuffer) / sizeof(char));

            requestBodyBuffer.assign(reqBodyStartChar, (reqBodyStartChar + bodyBufferSize));
            if ((contentLength = parseRequestHeaders()) == static_cast<size_t>(-1))
                return REQUEST_RECIEVE_FAILED;

            const size_t diff = headerEndChar - readBuffer;
            contentLength -= bytesRead - (diff + 4);
            if (contentLength == 0)
                break;
        }
        else
        {
            if (!isReadingHeader)
            {
                contentLength -= bytesRead;
                requestBodyBuffer.assign(readBuffer, readBuffer + bytesRead);
            }
            else
            {
                requestHeaderStream << readBuffer;
            }
            if (contentLength <= 0)
                break;
        }
    }
    return REQUEST_SAFE_STATE;
}

bool RequestHandler::startSending() const
{
    const int bytesSent = send(client_socket_fh, (char *)responseBuffer.data(), responseBuffer.size(), 0);
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
    const size_t start = s.find_first_not_of(" \t\r\n");
    const size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

int RequestHandler::parseRequestHeaders()
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
    return REQUEST_SAFE_STATE;
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

std::string RequestHandler::assignIpStr() const
{
    size_t addressSize = sizeof(server_addr.sin_addr.S_un.S_addr);
    char bytes[addressSize];

    strncpy(bytes, (char *)&server_addr.sin_addr.S_un.S_addr, addressSize);
    std::string ipStr;
    for (int i = 0; i < 4; i++)
    {
        ipStr += std::to_string(bytes[i]);
        if (i != 4 - 1)
            ipStr += ".";
    }
    return ipStr;
}