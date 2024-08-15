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
        res.setContentType("application/js");
        res.writeToBody(R"({"name": "Bob"})");
        return 0;
    }

    int serveGET(Request &req, Response &res) override
    {
        res.setContentType("text/html");
        std::ifstream file("../index.html");
        std::string line;
        if (file.is_open())
        {
            while (std::getline(file, line))
                res.writeToBody(std::string(line));
        }
        else
            std::cerr << "couldn't open file " << std::endl;
        return 0;
    }
};

int main()
{
    MyServer server;
    server.init();
    return 0;
}
