#include "../include/HttpRequest.hpp"
#include "../include/Logger.hpp"

Request::Request(
    std::unordered_map<std::string, std::string> &headerFields,
    std::stringstream &requestBodyStream)
    : headerFields(headerFields), requestBodyStream(requestBodyStream) {}

// optimizing this later
std::string Request::getMethod()
{
    auto foundField = headerFields.find("Method");
    if (foundField == headerFields.end())
        return "";
    return foundField->second;
}

std::string Request::getUrl()
{
    auto foundField = headerFields.find("Url");
    if (foundField == headerFields.end())
        return "";
    return foundField->second;
}

std::stringstream &Request::getBodyStream() { return requestBodyStream; }

std::string Request::getHeaderField(std::string key)
{
    auto foundField = headerFields.find(key);
    if (foundField == headerFields.end())
        return "";
    return foundField->second;
}
std::string Request::getContentType()
{
    auto foundField = headerFields.find("Content-Type");
    if (foundField == headerFields.end())
        return "";
    return foundField->second;
}
std::string Request::getHost()
{
    auto foundField = headerFields.find("Host");
    if (foundField == headerFields.end())
        return "";
    return foundField->second;
}