#pragma once

#include "AppIncludes.hpp"

class Response
{
private:
    std::string version = "HTTP/1.1";
    std::string status = "200";
    std::string reasonPhrase = "OK";

    std::unordered_map<std::string, std::string> headers;
    std::size_t contentLength = 0;

    bool isWriteComplete = false;
    std::stringstream responseStream;
    std::stringstream responseBodyStream;

    size_t responseBodySize = 0;
    std::vector<unsigned char> &responseBuffer;

    bool writeNecess();
    bool writeHeaders();
    bool writeFileToBody();
    bool writeStringToBody();

    size_t getFileSize() const;
    std::string getGMT() const;

    std::string filePath;

    bool isWriteStringToBodyMode = false;
    bool isWriteFileToBodyMode = false;

public:
    Response(std::vector<unsigned char> &responseBuffer);

    /*
        @brief: After this method invocation, the response is permanently set and ready to go,
                No further changes to the Response object will have an affect on the response being sent
        @note: Don't invoke this function while inside service phase
    */
    bool _startWriter();

    void setStatus(int statusCode);
    void setReasonPhrase(const std::string &reasonPhrase);
    void setContentType(const std::string &contentType);

    void setHeaderField(const std::string &key, const std::string &value);
    bool addHeaderField(const std::string &key, const std::string &nextValue);

    bool addHeaderFieldParam(const std::string &key, const std::string &parameter);

    void send(const std::string &contentStrng);

    void writeLine(const std::string &line);

    /*
        this will only set the file path, the writing-to-response will start after the '_startwriter()'
        @return 'true' if file exists, 'false' if file doesnt exits and if send() or writeLine() is invoked before sendFile()
    */
    bool sendFile(const std::string &filePath);
};
