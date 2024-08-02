GXX = g++
source = server.cpp
APPDIR = exe
APP = WebServer
TESTAPP = TestServer
SOCKET = ws2_32

SRC = Util/src


HTTPSERVER = HttpServer.cpp
LOGGER = Logger.cpp

all: build buildTest

util.o = $(SRC)/$(HTTPSERVER) $(SRC)/$(LOGGER)

build : $(source)
	$(GXX) -o $(APPDIR)/$(APP) $(util.o) -l$(SOCKET) $(source) 

buildTest : $(source)
	$(GXX) -o $(APPDIR)/$(TESTAPP) $(SRC)/$(HTTPSERVER) $(SRC)/$(LOGGER) -l$(SOCKET) $(source)  	 	

