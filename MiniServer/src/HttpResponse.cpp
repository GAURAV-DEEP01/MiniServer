#include "../include/HttpResponse.hpp"
#include "../include/Logger.hpp"

Response::Response(std::stringstream &responseStream) : headers(), responseStream(responseStream) {}

void Response::setStatus(int statusCode) { status = std::to_string(statusCode); }

void Response::setContentType(std::string contentType) { headers["Content-Type"] = contentType; }

void Response::setReasonPhrase(std::string reasonPhrase) { this->reasonPhrase = reasonPhrase; }

void Response::setAttribute(std::string key, std::string value)
{
    if (key == "Content-Type" || key == "Date" || key == "Server")
        return;
    headers[key] = value;
}

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
    headers["Server"] = "HTTPServer/1.0";
    headers["Date"] = getGMT();

    std::unordered_map<std::string, std::string>::iterator headerKeyValue;
    for (headerKeyValue = headers.begin(); headerKeyValue != headers.end(); headerKeyValue++)
        responseStream << headerKeyValue->first << ": " << headerKeyValue->second << "\r\n";
    responseStream << "\r\n";

    responseStream << responseBodyStream.str();
    Logger::info(responseStream.str());
    isWriteComplete = true;
}

std::string Response::getGMT()
{
    std::time_t now = std::time(nullptr);
    std::tm *gmtTime = std::gmtime(&now);

    std::stringstream gmtimeStream;
    gmtimeStream << std::put_time(gmtTime, "%a, %d %b %Y %H:%M:%S GMT");

    return gmtimeStream.str();
}