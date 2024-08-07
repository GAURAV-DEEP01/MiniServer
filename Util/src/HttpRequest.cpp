#include "../include/HttpRequest.hpp"
#include "../include/Logger.hpp"

Request::Request(
    std::unordered_map<std::string, std::string> &headerFields,
    std::stringstream &requestBodyStream)
    : headerFields(headerFields), requestBodyStream(requestBodyStream) {}

// to do... at this point everything is a to do
std::string Request::getMethod()
{
    return "getMethod";
}
std::string Request::getUrl()
{
    return "getUrl";
}
std::stringstream Request::getBodyStream()
{
    std::stringstream s("getBodyStream");
    return s;
}
std::string Request::getAttribute(std::string)
{
    return "getAttribute";
}
std::string Request::getContentType()
{
    return "getContentType";
}
std::string Request::getHost()
{
    return "getHost";
}