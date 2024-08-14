#include "miniserver.hpp"

class MyServer : public HttpServer
{
public:
    MyServer() : HttpServer() {}

    int middleWare(Request &req, Response &res) override
    {
        // middleware functions in order;
        return 0;
    }

    int servePOST(Request &req, Response &res) override
    {
        res.setContentType("text/html");
        res.writeToBody("<h1>post</h1>");
        return 0;
    }

    int serveGET(Request &req, Response &res) override
    {
        res.setContentType("text/html");
        res.writeToBody("<h1>get</h1>");
        return 0;
    }
};

int main()
{
    MyServer server;
    server.init();
    return 0;
}
