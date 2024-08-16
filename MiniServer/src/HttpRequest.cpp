#include "../include/HttpRequest.hpp"
#include "../include/Logger.hpp"

Request::Request(
    std::unordered_map<std::string, std::string> &headerFields,
    std::stringstream &requestBodyStream)
    : headerFields(headerFields), requestBodyStream(requestBodyStream)
{
    std::string url = getHeaderField("Url");
    std::size_t idx = url.find('?');
    if (idx == std::string::npos)
    {
        routeUrl = url;
        baseUrl = url;
    }
    else
    {
        baseUrl = url.substr(0, idx);
        routeUrl = url.substr(0, idx + 1);

        std::string queryParameterStr = url.substr(idx + 1);
        std::stringstream queryParamStream(queryParameterStr);
        std::string key, value;
        while (std::getline(queryParamStream, key, '='))
        {
            std::getline(queryParamStream, value, '&');
            queryParams[key] = value;
        }
    }
}

// optimizing this later
std::string Request::getMethod()
{
    return getHeaderField("Method");
}

std::string Request::getUrl()
{
    return getHeaderField("Url");
}

std::string Request::getBaseRouteUrl()
{
    return routeUrl;
}
std::string Request::getBaseUrl()
{
    return baseUrl;
}

std::string Request::getParameter(std::string key)
{
    auto foundParam = queryParams.find(key);
    if (foundParam == queryParams.end())
        return "";
    return foundParam->second;
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
    return getHeaderField("Content-Type");
}
std::string Request::getHost()
{
    return getHeaderField("Host");
}