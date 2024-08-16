# Mini Server

Mini Server is a lightweight C++ HTTP/1.1 server built from scratch it's easy modifiable and has simple routing API.

## Features

- **Lightweight HTTP Server:** Handles HTTP requests GET, POST, PUT, DELETE, POST and other methods using method SPECIFIC.
- **Connection keep-alive**: This is a HTTP/1.1 feature for persistant connection
- **Routing System:** Define routes with lambda functions.
- **Customizable Responses:** Set content type, status codes, and more.

## Quick Start

### Prerequisites

- C++17 or later
- A compatible compiler (G++)
- Make build tool

### Cloning the Repository

```cmd
git clone // ill add later
cd Mini-Server
```

### Building the Project

#### Using Make build the project

Compiles the whole project and will output `Webserver.exe` inside a `exe` folder in you project directory

```cmd
make build
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
HttpServer server;

/*
    Define route method (There are routes for other HTTP methods too)
    Make sure you have 'index.html'
*/
server.routeGet["/"] = [](Request &req, Response &res) -> int
{
    res.setContentType("text/html");
    res.sendFile("index.html");
    res.setStatus(200);
    return 0;
};

// Listen on a port (This method invokation should be after all the 'route' definition)
server.listen(9090);
```

Now, checkout `localhost:9090/` on your browser

## Documentation

- [How Routing Works](/Documentation/Routes.md)
- [How to use Miniserver as Library](/Documentation/Library.md)

## Contributing

Contributions are welcome! Please submit a pull request or open an issue to discuss any changes.

---

Feel free to customize the content to fit your project's specifics, such as adding more sections or examples.
