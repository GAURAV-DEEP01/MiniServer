GXX = g++
source = server.cpp
APPDIR = exe
APP = WebServer
TESTAPP = TestServer
SOCKET = ws2_32

SRC = Util/src


HTTPSERVER = HttpServer.cpp
LOGGER = Logger.cpp
REQUESTHANDLER = RequestHandler.cpp

all: build buildTest

util.o = $(SRC)/$(HTTPSERVER) $(SRC)/$(LOGGER) $(SRC)/$(REQUESTHANDLER)

build : $(source)
	$(GXX) -o $(APPDIR)/$(APP) $(util.o) -l$(SOCKET) $(source) 

buildTest : $(source)
	$(GXX) -DENABLE_LOGGER_INFO -o $(APPDIR)/$(TESTAPP) $(util.o) -l$(SOCKET) $(source)  	 	

