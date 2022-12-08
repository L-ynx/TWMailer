##
# TWMailer
#
# @file
# @version 0.1

CPPFLAGS := -Iinclude -Ilib -MMD -MP # C PreProcessor flags
CXXFLAGS := -std=c++20
CXXDEBUGFLAGS := -std=c++20 -Wall -Werror -pedantic-errors

SERVER := src/Socket.cpp src/Connection.cpp src/CommandHandler.cpp src/twmailer-server.cpp
CLIENT := src/Socket.cpp src/twmailer-client.cpp

all: bin server client

debug: bin server-debug client-debug

server:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SERVER) -o bin/twmailer-server

client:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CLIENT) -o bin/twmailer-client

server-debug:
	$(CXX) $(CPPFLAGS) $(CXXDEBUGFLAGS) $(SERVER) -o bin/twmailer-server

client-debug:
	$(CXX) $(CPPFLAGS) $(CXXDEBUGFLAGS) $(CLIENT) -o bin/twmailer-client

clean:
	-@rm -r bin obj 2>/dev/null || true

start-server:
	bin/twmailer-server 10001 test

start-client:
	bin/twmailer-client 127.0.0.1 10001

bin:
	mkdir bin

# end
