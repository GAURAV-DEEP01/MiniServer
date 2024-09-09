# Mini Server

Mini Server is a lightweight C++ HTTP/1.1 server built from scratch it's easily modifiable and has simple routing API.

## Features

- **Lightweight HTTP Server:** Handles HTTP requests GET, POST, PUT, DELETE, POST and other methods using method SPECIFIC.
- **Connection keep-alive**: This is a HTTP/1.1 feature for persistant connection
- **Routing System:** Define routes with lambda functions.
- **Customizable Responses:** Set content type, status codes, and more.

## Quick Start

### Prerequisites

- Windows
- C++17 or later
- A compatible compiler (G++)
- Make build tool

### Cloning the Repository

```cmd
git clone https://github.com/GAURAV-DEEP01/MiniServer.git
cd MiniServer
```

### Building the Project

#### Using Make build the project

Compiles the whole project and will output `Webserver.exe` inside a `exe` folder in you project directory

```cmd
make build
```

If you dont have Make build tool use this

step 1: create a directory exe

```cmd
mkdir -p exe
```

step 2: build the project

```cmd
g++ -o exe/WebServer MiniServer/src/HttpServer.cpp MiniServer/src/Logger.cpp MiniServer/src/RequestHandler.cpp MiniServer/src/HttpRequest.cpp MiniServer/src/HttpResponse.cpp -lws2_32 server.cpp -IMiniServer/include
```

- If you dont want to run the whole project, you can use it as a library instead view [How to use MiniServer as Library](/Documentation/Library.md)

### Running the Server

```cmd
./WebServer
```

## Simple Example Usage

You can define routes and handle requests using the following API:

```cpp
// Inside the main create a HttpServer instance
HttpServer app;


// Define route method (There are routes for other HTTP methods too)
app.Get("/",[](Request &req, Response &res) -> int
{
    res.setContentType("text/html");
    res.send("<h1>Server is running!<h1>");
    return 0;
});

// Listen on a port (This method invokation should be after all the 'route' definition)
app.listen(9090);
```

Now, checkout `localhost:9090/` on your browser

## Documentation

- [How does Routing and everything Work](/Documentation/Routes.md)
- [How to use Miniserver as Library](/Documentation/Library.md)

- Documentation for Windows socket API [winsock2.h](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/)

## Contributing

Contributions are welcome! Please submit a pull request or open an issue to discuss any changes.

---

Feel free to customize the content to fit your project's specifics, such as adding more sections or examples.
