##
# TWMailer
#
# @file
# @version 0.1

all: bin server client

server:
	g++ -Wall src/twmailer-server.cpp -o bin/twmailer-server

client:
	g++ -Wall src/twmailer-client.cpp -o bin/twmailer-client

clean:
	-@rm -r bin obj 2>/dev/null || true

bin:
	mkdir bin

# end
