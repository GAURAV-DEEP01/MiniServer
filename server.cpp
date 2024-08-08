#include "HttpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class MyServer : public HttpServer
{
public:
    MyServer() : HttpServer() {}

    // int service(Request &req, Response &res)
    // {
    //     res.setContentType("text/html;charset=utf-8");
    //     res.writeToBody("<h1>Testing...</h1>");
    //     return 0;
    // }

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
