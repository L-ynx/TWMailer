#include "Connection.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

Server::Server(int port, std::string directory) {
    this->port = port;
    this->ch = new CommandHandler(directory);
    this->serverSocket = new ServerSocket(port);
}

void Server::connect(Connection *client) {
    connection.push_back(client);
}

ServerSocket *Server::getSocket() {
    return this->serverSocket;
}

/**
 * Use the code from the example to communicate with the client. Doing it in an
 * object-oriented way makes the maintenance of the program simpler.
 * */
void Server::clientCommunication(Connection *conn) {
    char buffer[4096];
    int message;

    do {
        memset(buffer, 0, 4096);
        // Receive client message
        message = recv(*conn->getClientSocket(), buffer, 4096, 0);
        if (message == -1) {
            if (this->abortRequested) {
                perror("recv error after aborted");
            } else {
                perror("recv error");
            }
            break;
        }

        // at this point the stream is working, so it's time to do something with the
        // data
        ch->parseInput(std::string(buffer, 0, message));

        // std::cout << ch->getResponse().c_str() << std::endl;
        // std::cout << ch->getResponseLength() << std::endl;

        // send the response to the client
        send(*conn->getClientSocket(), ch->getResponse().c_str(),
             ch->getResponseLength(), 0);

    } while (strcasecmp(buffer, "QUIT") != 0 && !this->abortRequested &&
             message != 0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        Server::usage();
        return EXIT_FAILURE;
    }

    // Initialize Server
    Server server(atoi(argv[1]), argv[2]);

    // Establish and maintain connection
    while (true) {
        Connection *newClient = new Connection(*server.getSocket()->getSocket());
        // server.connect(newClient);
        std::thread t(&Server::clientCommunication, server, newClient);

        t.detach();
    }

    // not strictly necessary, but it's nice to be explicit about what we're doing
    close(*server.getSocket()->getSocket());
    return 0;
}
