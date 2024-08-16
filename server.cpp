#include "miniserver.hpp"

int main()
{
    HttpServer server;

    server.routeGet["/"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("text/html");
        res.sendFile("../public/index.html");
        res.setStatus(200);
        return 0;
    };

    server.routeGet["/style.css"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("text/css");
        res.sendFile("../public/style.css");
        return 0;
    };

    server.routeGet["/index.js"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("text/javascript");
        res.sendFile("../public/index.js");
        return 0;
    };

    server.routeGet["/favicon.ico"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("image/png");
        res.sendFile("../public/favicon.png");
        return 0;
    };

    server.listen(23000);
    return 0;
}
