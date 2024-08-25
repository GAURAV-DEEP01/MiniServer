#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "AppIncludes.hpp"

class Request
{
private:
    std::unordered_map<std::string, std::string> &headerFields;
    std::stringstream &requestBodyStream;
    std::unordered_map<std::string, std::string> queryParams;

    std::string url;
    std::string routeUrl;
    std::string baseUrl;

public:
    Request(std::unordered_map<std::string, std::string> &headerFields,
            std::stringstream &requestBodyStream);

    std::string getMethod();
    std::string getHost();

    std::string getUrl();
    std::string getBaseUrl();
    std::string getBaseRouteUrl();

    std::string getParameter(std::string key);
    std::stringstream &getBodyStream();
    std::string getHeaderField(std::string key);
    std::string getContentType();
};

#endif