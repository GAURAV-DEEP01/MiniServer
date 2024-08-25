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

SOURCEFILES = $(SRC)/$(HTTPSERVER) $(SRC)/$(LOGGER) $(SRC)/$(REQUESTHANDLER) $(SRC)/$(HTTPREQUEST) $(SRC)/$(HTTPRESPONSE)

build : makeExe clear $(SOURCE)
	$(GXX) -o $(APPDIR)/$(APP) $(SOURCEFILES) -l$(SOCKET) $(SOURCE) -I$(INCLUDE)

buildTest : clear $(SOURCE)
	$(GXX) -DENABLE_LOGGER_INFO -DNDEBUG -Wall -Wextra -o $(APPDIR)/$(TESTAPP) $(SOURCEFILES) -l$(SOCKET) $(SOURCE) -I$(INCLUDE)  	 	

LIBDLL = libminiserver.dll


$(LIBDLL) : $(SOURCEFILES)
	$(GXX) -shared -o $(LIB)/$(LIBDLL) $(SOURCEFILES) -l$(SOCKET)

serverLibTest : $(SOURCE)
	$(GXX) -DENABLE_LOGGER_INFO -o exe/LibTestServer $(SOURCE) -IMiniServer/include/ -LMiniServer/lib -lminiserver 


.PHONY: clear

makeExe: 
	mkdir -p exe

clear: 
	rm -f exe/* 
