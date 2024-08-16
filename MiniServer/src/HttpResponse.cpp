#include "../include/HttpResponse.hpp"
#include "../include/Logger.hpp"

Response::Response(std::vector<unsigned char> &responseBuffer) : responseBuffer(responseBuffer) {}

void Response::setStatus(int statusCode) { status = std::to_string(statusCode); }

void Response::setContentType(std::string contentType) { headers["Content-Type"] = contentType; }

void Response::setReasonPhrase(std::string reasonPhrase) { this->reasonPhrase = reasonPhrase; }

void Response::setHeaderField(std::string key, std::string value)
{
    if (key == "Content-Type" || key == "Date" || key == "Server")
        return;
    headers[key] = value;
}

void Response::writeToBody(std::string contentString)
{
    if (isfileWrite)
        return;
    contentLength += contentString.length();
    headers["Content-Length"] = std::to_string(contentLength);
    responseBodyStream << contentString;
    isWritebody = true;
}

bool Response::sendFile(std::string filePath)
{
    if (isfileWrite || isWritebody)
        return false;
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    std::stringstream stream;
    if (file.is_open())
    {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        responseBodybuffer.resize(size);
        file.read((char *)responseBodybuffer.data(), size);
        if (!file)
        {
            Logger::err("Unable to read file");
            return false;
        }
    }
    else
    {
        std::cerr << "Couldn't open file " << std::endl;
        return false;
    }
    file.close();
    isfileWrite = true;
    return true;
}

void Response::startWriter()
{
    if (isWriteComplete)
        return;
    responseStream << version << " " << status << " " << reasonPhrase << "\r\n";
    headers["Server"] = "HTTPServer/1.0";
    headers["Date"] = getGMT();

    if (isfileWrite)
    {
        contentLength = responseBodybuffer.size();
        headers["Content-Length"] = std::to_string(contentLength);
    }

    std::unordered_map<std::string, std::string>::iterator headerKeyValue;
    responseStream << "Connection: keep-alive\r\nKeep-Alive: timeout=10, max=100\r\n";
    for (headerKeyValue = headers.begin(); headerKeyValue != headers.end(); headerKeyValue++)
        responseStream << headerKeyValue->first << ": " << headerKeyValue->second << "\r\n";
    responseStream << "\r\n";
    if (!isfileWrite && isWritebody)
        responseStream << responseBodyStream.str();

    responseStream.seekg(0, std::ios::end);
    std::streamsize size = responseStream.tellg();
    responseStream.seekg(0, std::ios::beg);
    if (size < 0)
    {
        Logger::err("Failed to determine stream size.");
        return;
    }
    responseBuffer.resize(size);
    responseStream.read((char *)responseBuffer.data(), responseBuffer.size());
    if (!responseStream)
        Logger::err("response read failed");

    if (isfileWrite)
    {
        responseBuffer.insert(responseBuffer.end(), responseBodybuffer.begin(), responseBodybuffer.end());
    }
    isWriteComplete = true;
}

std::string Response::getGMT()
{
    std::time_t now = std::time(nullptr);
    std::tm *gmtTime = std::gmtime(&now);

    std::stringstream gmtimeStream;
    gmtimeStream << std::put_time(gmtTime, "%a, %d %b %Y %H:%M:%S GMT");

    return gmtimeStream.str();
}