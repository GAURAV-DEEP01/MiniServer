#include "../include/HttpResponse.hpp"
#include "../include/Logger.hpp"

Response::Response(std::stringstream &responseStream) : headers(), responseStream(responseStream) {}

void Response::setStatus(int statusCode) { status = std::to_string(statusCode); }

void Response::setContentType(std::string contentType) { headers["Content-Type"] = contentType; }

void Response::setReasonPhrase(std::string reasonPhrase) { this->reasonPhrase = reasonPhrase; }

void Response::setAttribute(std::string key, std::string value) { headers[key] = value; }

void Response::writeToBody(std::string contentString)
{
    contentLength += contentString.length();
    headers["Content-Length"] = std::to_string(contentLength);
    responseBodyStream << contentString;
}

void Response::startWriter()
{
    if (isWriteComplete)
        return;
    responseStream << version << " " << status << " " << reasonPhrase << "\r\n";
    std::unordered_map<std::string, std::string>::iterator headerKeyValue;
    responseStream << "Server: GDHTTPServer/1.0\r\n";
    for (headerKeyValue = headers.begin(); headerKeyValue != headers.end(); headerKeyValue++)
        responseStream << headerKeyValue->first << ": " << headerKeyValue->second << "\r\n";

    responseStream << "Date: Fri, 02 Aug 2024 10:00:00 GMTr\n";

    responseStream << "\r\n";
    responseStream << responseBodyStream.str();
    Logger::info(responseStream.str());
    isWriteComplete = true;
}