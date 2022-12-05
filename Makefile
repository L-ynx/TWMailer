##
# TWMailer
#
# @file
# @version 0.1

CPPFLAGS := -Iinclude -Ilib -MMD -MP # C PreProcessor flags
CXXFLAGS := -std=c++20 -Wall -Werror -pedantic-errors

all: bin server client

server:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/twmailer-server.cpp -o bin/twmailer-server

client:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/twmailer-client.cpp -o bin/twmailer-client

clean:
	-@rm -r bin obj 2>/dev/null || true

bin:
	mkdir bin

# end
