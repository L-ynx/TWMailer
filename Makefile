##
# TWMailer
#
# @file
# @version 0.1

CPPFLAGS := -Iinclude -Ilib -MMD -MP # C PreProcessor flags
CXXFLAGS := -std=c++20 -Wall -Werror -pedantic-errors

SERVER := src/ServerSocket.cpp src/Connection.cpp src/twmailer-server.cpp
CLIENT := src/twmailer-client.cpp

all: bin server client

server:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SERVER) -o bin/twmailer-server

client:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CLIENT) -o bin/twmailer-client

clean:
	-@rm -r bin obj 2>/dev/null || true

start-server:
	bin/twmailer-server 10001 test

start-client:
	bin/twmailer-client 127.0.0.1 10001

bin:
	mkdir bin

# end