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
        res.setContentType("text/html");
        res.writeToBody("<h1>Server Running...</h1>");
        return 0;
    }
};

int main()
{
    MyServer server;
    server.init();
    return 0;
}
