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
    std::stringstream responseStream;

    std::vector<unsigned char> &responseBuffer;
    std::vector<unsigned char> responseBodybuffer;

    std::string getGMT();

    bool isWritebody = false;
    bool isfileWrite = false;

public:
    Response(std::vector<unsigned char> &responseBuffer);

    // sets reponse status to given integer status
    void setStatus(int statusCode);
    void setContentType(std::string contentType);
    void setReasonPhrase(std::string reasonPhrase);
    void setHeaderField(std::string key, std::string value);
    void writeToBody(std::string contentStrng);
    void startWriter();
    bool sendFile(std::string filePath);
};

#endif