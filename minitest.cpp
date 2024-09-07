#include "miniserver.hpp"

int main()
{
    HttpServer app;

    app.Get("/",[](Request &, Response &res) -> int
    {
        res.setContentType("text/html");
        res.send("<h1>Hello from Server!<h1>");
        return 0;
    });

    app.listen(9090);
    return 0;
}