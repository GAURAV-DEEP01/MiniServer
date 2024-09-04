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

    if (isWriteStringToBodyMode)
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
    headers["Server"] = "MiniServer/1.0";
    headers["Date"] = getGMT();

    if (isWriteFileToBodyMode)
    {
        if ((contentLength = getFileSize()) == ULLONG_MAX)
            return false;
        responseBodySize = contentLength;
        headers["Content-Length"] = std::to_string(responseBodySize);
    }
    else
    {
        responseBodySize = responseBodyStream.tellp();
        headers["Content-Length"] = std::to_string(responseBodySize);
    }
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

    size_t size = responseStream.tellp();
    if (size == static_cast<size_t>(-1))
    {
        Logger::err("Failed to determine response stream size.");
        return false;
    }

    responseBuffer.resize(size + responseBodySize);
    responseStream.read((char *)responseBuffer.data(), size);
    if (!responseStream)
    {
        Logger::err("Response read failed");
        return false;
    }
    return true;
}

bool Response::writeStringToBody()
{
    std::streamsize responseHeaderSize = responseStream.tellp();

    responseBodyStream.read((char *)&responseBuffer[responseHeaderSize], responseBodySize);
    if (!responseBodyStream)
    {
        Logger::err("Unable to write string to body");
        return false;
    }
    return true;
}

bool Response::writeFileToBody()
{
    std::ifstream file(filePath, std::ios::binary);
    if (file.is_open())
    {
        size_t responseHeaderSize = responseStream.tellp();
        if (responseHeaderSize == (size_t)-1)
        {
            Logger::err("Failed to determine response header size");
            return false;
        }
        file.read((char *)&responseBuffer[responseHeaderSize], responseBodySize);
        if (!file)
        {
            Logger::err("Unable to read file " + std::to_string(file.gcount()));
            file.close();
            return false;
        }

        file.close();
        return true;
    }
    Logger::err("Couldn't open file " + filePath);
    file.close();
    return false;
}

void Response::send(const std::string &contentString)
{
    if (isWriteFileToBodyMode)
        return;

    responseBodyStream << contentString;
    isWriteStringToBodyMode = true;
}

void Response::writeLine(const std::string &line)
{
    if (isWriteFileToBodyMode)
        return;
    responseBodyStream << line << "\n";
    isWriteStringToBodyMode = true;
}

bool Response::sendFile(const std::string &filePath)
{
    if (isWriteFileToBodyMode || isWriteStringToBodyMode)
        return false;
    if (!std::filesystem::exists(filePath))
        return false;
    this->filePath = filePath;
    isWriteFileToBodyMode = true;

    return true;
}

bool Response::addHeaderField(const std::string &key, const std::string &nextValue)
{
    auto foundField = headers.find(key);
    if (foundField == headers.end())
        return false;

    foundField->second += ", " + nextValue;
    return true;
}

bool Response::addHeaderFieldParam(const std::string &key, const std::string &parameter)
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

void Response::setContentType(const std::string &contentType)
{
    headers["Content-Type"] = contentType;
}

void Response::setReasonPhrase(const std::string &reasonPhrase)
{
    this->reasonPhrase = reasonPhrase;
}

void Response::setHeaderField(const std::string &key, const std::string &value)
{
    if (key == "Content-Type" || key == "Date" || key == "Server")
        return;
    headers[key] = value;
}

std::string Response::getGMT() const
{
    std::time_t now = std::time(nullptr);
    std::tm *gmtTime = std::gmtime(&now);

    std::stringstream gmtimeStream;
    gmtimeStream << std::put_time(gmtTime, "%a, %d %b %Y %H:%M:%S GMT");

    return gmtimeStream.str();
}

size_t Response::getFileSize() const
{
    if (!std::filesystem::exists(filePath))
    {
        Logger::err("File '" + filePath + "', Does not exitsts");
        return -1;
    }

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        Logger::err("Could'nt open file " + filePath);
        return -1;
    }

    std::streamsize size = file.tellg();
    if (size < 0)
    {
        Logger::err("Unable to determine size of file " + filePath);
        file.close();
        return -1;
    }
    file.close();

    return size;
}