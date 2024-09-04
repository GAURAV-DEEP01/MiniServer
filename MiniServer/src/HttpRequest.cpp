#include "../include/HttpRequest.hpp"
#include "../include/Logger.hpp"

Request::Request(
    const std::unordered_map<std::string, std::string> &headerFields,
    const std::stringstream &requestBodyStream)
    : headerFields(headerFields),
      requestBodyStream(requestBodyStream)
{
    queryParams = assignQueryParams();
}

std::unordered_map<std::string, std::string> Request::assignQueryParams()
{
    std::unordered_map<std::string, std::string> queryParameters;
    std::string url = getUrl();
    size_t idx = url.find('?');
    if (idx == std::string::npos)
    {
        baseUrl = url;
        routeUrl = url;
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
            queryParameters[key] = value;
        }
    }
    return queryParameters;
}
std::string Request::getMethod() const
{
    return getHeaderField("Method");
}

std::string Request::getUrl() const
{
    return getHeaderField("Url");
}

std::string Request::getBaseRouteUrl() const
{
    return routeUrl;
}
std::string Request::getBaseUrl() const
{
    return baseUrl;
}

std::string Request::getParameter(const std::string &key) const
{
    auto foundParam = queryParams.find(key);
    if (foundParam == queryParams.end())
        return "";
    return foundParam->second;
}

const std::stringstream &Request::getBodyStream() const { return requestBodyStream; }

std::string Request::getHeaderField(const std::string &key) const
{
    auto foundField = headerFields.find(key);
    if (foundField == headerFields.end())
        return "";
    return foundField->second;
}
std::string Request::getContentType() const
{
    return getHeaderField("Content-Type");
}
std::string Request::getHost() const
{
    return getHeaderField("Host");
}