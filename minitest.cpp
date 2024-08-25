#include "miniserver.hpp"

int main()
{
    HttpServer app;

    app.routeGet["/"] = [](Request &req, Response &res) -> int
    {
        res.setContentType("text/html");
        res.send("<h1>Hello from Server!<h1>");
        return 0;
    };

    app.listen(9090);
    return 0;
}