GXX = g++
SOURCE = server.cpp
APPDIR = exe
APP = WebServer
TESTAPP = TestServer
SOCKET = ws2_32

SRC = MiniServer/src
INCLUDE = MiniServer/include
LIB = MiniServer/lib

HTTPSERVER = HttpServer.cpp
LOGGER = Logger.cpp
REQUESTHANDLER = RequestHandler.cpp
HTTPREQUEST = HttpRequest.cpp
HTTPRESPONSE = HttpResponse.cpp

all: build buildTest

util.o = $(SRC)/$(HTTPSERVER) $(SRC)/$(LOGGER) $(SRC)/$(REQUESTHANDLER) $(SRC)/$(HTTPREQUEST) $(SRC)/$(HTTPRESPONSE)

build : clear $(SOURCE)
	$(GXX) -o $(APPDIR)/$(APP) $(util.o) -l$(SOCKET) $(SOURCE) -I$(INCLUDE)

buildTest : clear $(SOURCE)
	$(GXX) -DENABLE_LOGGER_INFO -Wall -Wextra -o $(APPDIR)/$(TESTAPP) $(util.o) -l$(SOCKET) $(SOURCE) -I$(INCLUDE)  	 	

# ill definitly add comments here later....

LIBTARGETS = $(SRC)/$(HTTPSERVER) $(SRC)/$(LOGGER) $(SRC)/$(REQUESTHANDLER) $(SRC)/$(HTTPREQUEST) $(SRC)/$(HTTPRESPONSE)
LIBDLL = libminiserver.dll


$(LIBDLL) : $(LIBTARGETS)
	$(GXX) -shared -o $(LIB)/$(LIBDLL) $(LIBTARGETS) -l$(SOCKET)

.PHONY: clear

clear: 
	rm -f exe/WebServer && rm -f exe/TestServer
