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

    server.routeGet["/script.js"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("text/javascript");
        res.sendFile("../public/script.js");
        return 0;
    };

    server.routeGet["/favicon.ico"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("image/png");
        res.sendFile("../public/favicon.png");
        return 0;
    };

    server.routeGet["/status"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("text/plain");
        res.writeToBody("Server is up and running...");
        return 0;
    };

    server.routeGet["/github-logo.png"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("image/png");
        res.sendFile("../public/github-logo.png");
        return 0;
    };

    server.routeGet["/items?"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("text/html");
        res.writeToBody("<h1> id: " + req.getParameter("id") + "</h1>");
        return 0;
    };

    server.listen(23000);
    return 0;
}
