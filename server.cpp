#include "Util/include/AppIncludes.hpp"
#include "Util/include/Logger.hpp"
#include "Util/include/HttpServer.hpp"

class MyServer : public HttpServer
{
public:
    MyServer() : HttpServer() {}

    int service() override
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