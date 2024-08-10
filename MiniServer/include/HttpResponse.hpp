#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "AppIncludes.hpp"

class Response
{
private:
    // status line default values
    std::string status = "200";
    std::string version = "HTTP/1.1";
    std::string reasonPhrase = "OK";

    // headers
    std::unordered_map<std::string, std::string> headers;
    std::stringstream responseBodyStream;
    std::size_t contentLength = 0;

    // whole response
    bool isWriteComplete = false;
    std::stringstream &responseStream;

    std::string getGMT();

public:
    // will add detailed comments lator...
    Response(std::stringstream &responseStream);
    void setStatus(int statusCode);
    void setContentType(std::string contentType);
    void setReasonPhrase(std::string reasonPhrase);
    void setAttribute(std::string key, std::string value);
    void writeToBody(std::string contentStrng);
    void startWriter();
};

#endif