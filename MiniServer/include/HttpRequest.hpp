#pragma once

#include "AppIncludes.hpp"

class Request
{
private:
    const std::unordered_map<std::string, std::string> &headerFields;
    const std::vector<unsigned char> &requestBodyBuffer;
    std::unordered_map<std::string, std::string> queryParams;

    std::string routeUrl;
    std::string baseUrl;

    std::unordered_map<std::string, std::string> assignQueryParams();

public:
    Request(const std::unordered_map<std::string, std::string> &headerFields,
            const std::vector<unsigned char> &requestBodyBuffer);

    std::string getMethod() const;
    std::string getHost() const;

    std::string getUrl() const;
    std::string getBaseUrl() const;
    std::string getBaseRouteUrl() const;

    std::string getParameter(const std::string &key) const;
    const std::vector<unsigned char> &getBody() const;
    std::string getHeaderField(const std::string &key) const;
    std::string getContentType() const;
};
