GXX = g++
source = server.cpp
APPDIR = exe
APP = WebServer
TESTAPP = TestServer
SOCKET = ws2_32

SRC = MiniServer/src
INCLUDE = MiniServer/include

HTTPSERVER = HttpServer.cpp
LOGGER = Logger.cpp
REQUESTHANDLER = RequestHandler.cpp
HTTPREQUEST = HttpRequest.cpp
HTTPRESPONSE = HttpResponse.cpp

all: build buildTest

util.o = $(SRC)/$(HTTPSERVER) $(SRC)/$(LOGGER) $(SRC)/$(REQUESTHANDLER) $(SRC)/$(HTTPREQUEST) $(SRC)/$(HTTPRESPONSE)

build : $(source)
	$(GXX) -o $(APPDIR)/$(APP) $(util.o) -l$(SOCKET) $(source) -I$(INCLUDE)

buildTest : $(source)
	$(GXX) -DENABLE_LOGGER_INFO -o $(APPDIR)/$(TESTAPP) $(util.o) -l$(SOCKET) $(source) -I$(INCLUDE)  	 	

