# How does Routing and everything Work?

## Overview

This document provides an in-depth explanation of the routing system and other core functionalities in my custom HTTP server, MiniServer. This server allows you to define routes, handle HTTP requests, and generate appropriate responses. The server supports common HTTP methods such as GET, POST, PUT, PATCH, and DELETE.

## Routing System

The routing system is designed to map URL paths to specific handler functions. Each handler function takes two parameters: a `Request` object representing the client's request and a `Response` object used to construct the server's response.

### Defining Routes

Routes are defined using methods corresponding to HTTP verbs. For example, to define a route that responds to GET requests, you can use the `Get` method:

```cpp
app.Get("/path", [](Request &req, Response &res) -> int {
    // Handler code here
    return 0;
});
```

#### Example on where to define routes 
Include the MiniServer header file
```cpp
#include "miniserver.hpp"
```

In the main, Create an instance of the HttpServer and listen on a port after all your route defintions

```cpp
int main(){
    HttpServer app;

    // define all routes here

    app.listen(9090);
    return 0;
}
```

Sample program

```cpp
#include "miniserver.hpp"

int main()
{
    HttpServer app;

    app.Get("/",[](Request &req, Response &res) -> int 
    {
        res.setContentType("text/html");
        res.send("<h1>Hello from Server!<h1>");
        return 0;
    });

    app.listen(9090);
    return 0;
}
```
compile the program and run: [Compile the whole project - view Readme](../README.md) or [Use MiniServer as a library](./Library.md)

And done!, Now your server is listening to the port 9090 go to your browser or any tool and search `localhost:9090`

![Localhost image](images/browser.png)

You can view the connection made by the clients through the logs

![Mini Server Logs](images/logs.png)

The server supports the following methods to define routes:

- `Get(const std::string &path, const std::function<int(Request &, Response &)> &handler)`
- `Post(const std::string &path, const std::function<int(Request &, Response &)> &handler)`
- `Put(const std::string &path, const std::function<int(Request &, Response &)> &handler)`
- `Patch(const std::string &path, const std::function<int(Request &, Response &)> &handler)`
- `Delete(const std::string &path, const std::function<int(Request &, Response &)> &handler)`
- `Specific(const std::string &path, const std::function<int(Request &, Response &)> &handler)`

### Routing Internals

The routing methods store handler functions in unordered maps where the key is the route path, and the value is the handler function. When an incoming request is received, the server extracts the HTTP method and URL path, then looks up the corresponding handler in the relevant map. If a handler is found, it is invoked with the `Request` and `Response` objects.

### Handling Not Found Routes

If a route is not found, the server invokes the `defaultService` method, which returns a 404 status code with a "Not Found" message.

### Query Parameters

Query parameters in URLs (e.g., `/items?id=123`) are parsed by the `Request` class and can be accessed using the `getParameter` method. This allows you to extract specific data from the URL query string.

### Example

Here's a simple example demonstrating how to define routes and handle query parameters:

```cpp
app.Post("/items?", [](Request &req, Response &res) -> int {
    res.setContentType("text/html");
    res.send("<h1> id: " + req.getParameter("id") + "</h1>");
    return 0;
});
```

## Request Class

The `Request` class encapsulates all the information about an incoming HTTP request. It provides methods to access the request method, headers, body, URL, and query parameters.

### Key Methods

- `getMethod() const`: Returns the HTTP method (e.g., "GET", "POST").
- `getUrl() const`: Returns the full URL requested.
- `getBaseUrl() const`: Returns the base URL without query parameters.
- `getParameter(const std::string &key) const`: Retrieves a query parameter by key.
- `getHeaderField(const std::string &key) const`: Returns the value of a specific header.
- `getBody() const`: Returns the request body as a vector of bytes.

## Response Class

The `Response` class is responsible for constructing and sending the HTTP response back to the client. It allows setting status codes, headers, content type, and body content.

### Key Methods

- `setStatus(int statusCode)`: Sets the HTTP status code.
- `setReasonPhrase(const std::string &reasonPhrase)`: Sets the reason phrase corresponding to the status code.
- `setContentType(const std::string &contentType)`: Sets the content type of the response.
- `setHeaderField(const std::string &key, const std::string &value)`: Sets a specific header field.
- `bool addHeaderField(const std::string &key, const std::string &nextValue)`: Adds an extra Header Field, (e.g., name : value1, value2) here value2 is added
- `bool addHeaderFieldParam(const std::string &key, const std::string &parameter)`: Adds a paramter for the header field, (e.g., name : value; parameter) 
- `send(const std::string &contentStrng)`: Writes a string as the response body, you can call this function mulitiple times data is written to response at the end.
- `writeLine(const std::string &line)`: Writes a string to the response body and adds a new line
- `sendFile(const std::string &filePath)`: Sends a file as the response body.


## Server Internals

The server manages incoming connections using a multi-threaded approach. Each client connection is handled in a separate thread to allow concurrent request processing. 

### Connection Management

- `listen(short port)`: Initializes the server, sets up the TCP connection, and starts listening for incoming connections.
- `requestAcceptor()`: Continuously accepts incoming client connections and spawns a new thread to handle each request.

### Request Handling

The `reqInstantiator` method creates an instance of `RequestHandler` for each incoming request. The `RequestHandler` object then processes the request by invoking the appropriate route handler.

### Middleware

The server supports middleware functions that can be used to modify requests and responses before they are processed by route handlers. The middleware function is defined as a lambda function and passed to the server during initialization.

## Conclusion

This document provides an overview of how routing, request handling, and response generation work in the custom HTTP server. The server is designed to be lightweight yet powerful, Handles request with query parameters, and has different HTTP methods. By using this documentation, you should be able to extend the server's functionality or integrate it into your projects. Note: the server does not compress the body data before sending and there is more feature/requirement to the server yet to implement or not implemented.

 