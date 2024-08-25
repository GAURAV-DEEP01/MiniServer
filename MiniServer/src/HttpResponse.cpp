#include "../include/HttpResponse.hpp"
#include "../include/Logger.hpp"

Response::Response(std::vector<unsigned char> &responseBuffer) : responseBuffer(responseBuffer) {}

bool Response::_startWriter()
{
    if (isWriteComplete)
        return true;

    writeNecess();

    if (!writeHeaders())
        return false;

    if (isStringWrittenToBody)
    {
        if (!writeStringToBody())
            return false;
    }
    else if (!writeFileToBody())
        return false;

    isWriteComplete = true;
    return true;
}

bool Response::writeNecess()
{
    responseStream << version << " " << status << " " << reasonPhrase << "\r\n";
    headers["Server"] = "HTTPServer/1.0";
    headers["Date"] = getGMT();

    if (isFileWrittenToBody)
    {
        if ((contentLength = getFileSize()) == ULONG_MAX)
            return false;
        headers["Content-Length"] = std::to_string(contentLength);
    }
    else
        headers["Content-Length"] = std::to_string(responseBodyStream.tellp());

    setHeaderField("Connection", "keep-alive");
    setHeaderField("Keep-Alive", "timeout=10, max=100");

    return true;
}

bool Response::writeHeaders()
{
    std::unordered_map<std::string, std::string>::iterator headerKeyValue;
    for (headerKeyValue = headers.begin(); headerKeyValue != headers.end(); headerKeyValue++)
        responseStream << headerKeyValue->first << ": " << headerKeyValue->second << "\r\n";
    responseStream << "\r\n";

    std::streamsize size = responseStream.tellp();
    if (size < 0)
    {
        Logger::err("Failed to determine stream size.");
        return false;
    }

    responseBuffer.resize(size);
    responseStream.read((char *)responseBuffer.data(), responseBuffer.size());
    if (!responseStream)
    {
        Logger::err("response read failed");
        return false;
    }
    return true;
}

bool Response::writeStringToBody()
{
    std::streamsize size = responseBodyStream.tellp();
    std::streamsize responseHeaderSize = responseBuffer.size();

    responseBuffer.resize(responseHeaderSize + size);

    responseBodyStream.read((char *)&responseBuffer[responseHeaderSize], size);
    if (!responseBodyStream)
    {
        Logger::err("Unable to write string to body");
        return false;
    }
    return true;
}

bool Response::writeFileToBody()
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::size_t responseHeaderSize = responseBuffer.size();
        responseBuffer.resize(responseHeaderSize + size);

        file.read((char *)&responseBuffer[responseHeaderSize], size);
        if (!file)
        {
            Logger::err("Unable to read file");
            file.close();
            return false;
        }
    }
    else
    {
        Logger::err("Couldn't open file " + filePath);
        file.close();
        return false;
    }
    file.close();
    return true;
}

void Response::send(std::string contentString)
{
    if (isFileWrittenToBody)
        return;

    responseBodyStream << contentString;
    isStringWrittenToBody = true;
}

bool Response::sendFile(std::string filePath)
{
    if (isFileWrittenToBody || isStringWrittenToBody)
        return false;

    this->filePath = filePath;
    isFileWrittenToBody = true;

    if (!_startWriter())
        return false;
    return true;
}

bool Response::addHeaderField(std::string key, std::string nextValue)
{
    auto foundField = headers.find(key);
    if (foundField == headers.end())
        return false;

    foundField->second += ", " + nextValue;
    return true;
}

bool Response::addHeaderFieldParam(std::string key, std::string parameter)
{
    auto foundField = headers.find(key);
    if (foundField == headers.end())
        return false;

    foundField->second += "; " + parameter;
    return true;
}

void Response::setStatus(int statusCode)
{
    status = std::to_string(statusCode);
}

void Response::setContentType(std::string contentType)
{
    headers["Content-Type"] = contentType;
}

void Response::setReasonPhrase(std::string reasonPhrase)
{
    this->reasonPhrase = reasonPhrase;
}

void Response::setHeaderField(std::string key, std::string value)
{
    if (key == "Content-Type" || key == "Date" || key == "Server")
        return;
    headers[key] = value;
}

std::string Response::getGMT()
{
    std::time_t now = std::time(nullptr);
    std::tm *gmtTime = std::gmtime(&now);

    std::stringstream gmtimeStream;
    gmtimeStream << std::put_time(gmtTime, "%a, %d %b %Y %H:%M:%S GMT");

    return gmtimeStream.str();
}

size_t Response::getFileSize()
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!std::filesystem::exists(filePath))
    {
        Logger::err("File '" + filePath + "', Does not exitsts");
        return -1;
    }
    if (!file.is_open())
        return -1;

    std::streamsize size = file.tellg();
    file.close();

    return size;
}