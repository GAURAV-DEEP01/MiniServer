#include "../include/HttpRequest.hpp"
#include "../include/Logger.hpp"

Request::Request(
    std::unordered_map<std::string, std::string> &headerFields,
    std::stringstream &requestBodyStream)
    : headerFields(headerFields), requestBodyStream(requestBodyStream) {}

// to do... at this point everything is a to do
std::string Request::getMethod() { return headerFields["Method"]; }

std::string Request::getUrl() { return headerFields["Url"]; }

std::stringstream Request::getBodyStream()
{
    std::stringstream s("getBodyStream");
    return s;
}
std::string Request::getHeaderField(std::string)
{
    return "getHeaderFilds";
}
std::string Request::getContentType()
{
    return "getContentType";
}
std::string Request::getHost()
{
    return "getHost";
}