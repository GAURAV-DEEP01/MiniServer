#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "AppIncludes.hpp"

class Response
{
private:
    // status line default values
    std::string version = "HTTP/1.1";
    std::string status = "200";
    std::string reasonPhrase = "OK";

    // headers
    std::unordered_map<std::string, std::string> headers;
    std::stringstream responseBodyStream;
    std::size_t contentLength = 0;

    // whole response
    bool isWriteComplete = false;
    std::stringstream responseStream;

    std::vector<unsigned char> &responseBuffer;

    bool writeNecess();
    bool writeHeaders();
    bool writeFileToBody();
    bool writeStringToBody();

    size_t getFileSize();
    std::string getGMT();

    std::string filePath;

    bool isStringWrittenToBody = false;
    bool isFileWrittenToBody = false;

public:
    Response(std::vector<unsigned char> &responseBuffer);

    bool _startWriter();

    void setStatus(int statusCode);
    void setReasonPhrase(std::string reasonPhrase);
    void setContentType(std::string contentType);

    void setHeaderField(std::string key, std::string value);
    bool addHeaderField(std::string key, std::string nextValue);

    bool addHeaderFieldParam(std::string key, std::string parameter);

    void send(std::string contentStrng);

    /*
        this will trigger 'startWriter()' so after 'sendFile()' method no further modification to response object will have an effect
        @return 'true' if send successfull 'false' if send faild and will display the error message
    */
    bool sendFile(std::string filePath);
};

#endif