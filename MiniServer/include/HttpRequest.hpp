#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "AppIncludes.hpp"

class Request
{
private:
    std::unordered_map<std::string, std::string> &headerFields;
    std::stringstream &requestBodyStream;

public:
    Request(std::unordered_map<std::string, std::string> &headerFields,
            std::stringstream &requestBodyStream);

    std::string getMethod();
    std::string getUrl();
    std::stringstream getBodyStream();
    std::string getAttribute(std::string key);
    std::string getContentType();
    std::string getHost();
};

#endif