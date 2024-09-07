#include "miniserver.hpp"

int main()
{
    HttpServer app;

    app.Get("/", [](Request &, Response &res) -> int
    {
        res.setContentType("text/html");
        res.sendFile("../public/index.html");
        return 0;     
    });

    app.Get("/style.css", [](Request &, Response &res) -> int
    {    
        res.setContentType("text/css");
        res.sendFile("../public/style.css");
        return 0;
    });

    app.Get("/script.js", [](Request &, Response &res) -> int
    {
        res.setContentType("text/javascript");
        res.sendFile("../public/script.js");
        return 0;
    });

    app.Get("/favicon.ico", [](Request &, Response &res) -> int
    {
        res.setContentType("image/png");
        res.sendFile("../public/favicon.png");
        return 0;
    });

    app.Get("/status", [](Request &, Response &res) -> int
    {
        res.setContentType("text/plain");
        res.send("Server is up and running...");
        return 0;
    });

    app.Get("/github-logo.png", [](Request &, Response &res) -> int
    {
        res.setContentType("image/png");
        res.sendFile("../public/github-logo.png");
        return 0;
    });

    app.Get("/linkedin-logo.png", [](Request &, Response &res) -> int
    {
        res.setContentType("image/png");
        res.sendFile("../public/linkedin-logo.png");
        return 0;
    });

    // query parameter handling demo
    app.Post("/items?", [](Request &req, Response &res) -> int
    {
        res.setContentType("text/html");
        res.send("<h1> id: " + req.getParameter("id") + "</h1>");
        return 0;
    });

    app.listen(23000);
    return 0;
}
