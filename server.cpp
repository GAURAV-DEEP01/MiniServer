#include "Util/include/AppIncludes.hpp"
#include "Util/include/Logger.hpp"
#include "Util/include/HttpServer.hpp"
#include "Util/include/HttpRequest.hpp"
#include "Util/include/HttpResponse.hpp"

class MyServer : public HttpServer
{
public:
    MyServer() : HttpServer() {}

    int service(Request &req, Response &res) override
    {
        Logger::info("Service Ready");
        return 0;
    }
};

int main()
{
    MyServer server;
    server.init();
    return 0;
}