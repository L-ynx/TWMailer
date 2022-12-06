#include "Connection.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, std::string directory) {
    this->port = port;
    this->ch = new CommandHandler(directory);
    this->serverSocket = new ServerSocket(port);
}

void Server::connect(Connection *client) {
    this->connection = client;
}

Connection *Server::getConnection() {
    return this->connection;
}

ServerSocket *Server::getSocket() {
    return this->serverSocket;
}

/**
 * Use the code from the example to communicate with the client. Doing it in an
 * object-oriented way makes the maintenance of the program simpler.
 * */
void Server::clientCommunication() {
    char buffer[4096];
    int message;

    do {
        memset(buffer, 0, 4096);
        message = recv(*this->connection->getClientSocket(), buffer, 4096, 0);
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

        std::cout << ch->getResponse().c_str() << std::endl;
        std::cout << ch->getResponseLength() << std::endl;

        // send the response to the client
        send(*this->connection->getClientSocket(), ch->getResponse().c_str(),
             ch->getResponseLength(), 0);

    } while (strcasecmp(buffer, "QUIT") != 0 && !this->abortRequested &&
             message != 0);
    close(*this->connection->getClientSocket());
}

void Server::signalHandler(int sig) {
    if (sig == SIGINT) {
        std::cout << "abort Requested... ";
        this->abortRequested = 1;

        if (*this->connection->getClientSocket() != -1) {
            if (shutdown(*this->connection->getClientSocket(), SHUT_RDWR) == -1) {
                perror("shutdown *this->connection->getClientSocket()");
            }
            if (close(*this->connection->getClientSocket()) == -1) {
                perror("close *this->connection->getClientSocket()");
            }
            *this->connection->getClientSocket() = -1;
        }

        if (*this->serverSocket->getSocket() != -1) {
            if (shutdown(*this->serverSocket->getSocket(), SHUT_RDWR) == -1) {
                perror("shutdown *this->serverSocket->getSocket()");
            }
            if (close(*this->serverSocket->getSocket()) == -1) {
                perror("close *this->serverSocket->getSocket()");
            }
            *this->serverSocket->getSocket() = -1;
        }
    } else {
        exit(sig);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        Server::usage();
        return EXIT_FAILURE;
    }

    Server server(atoi(argv[1]), argv[2]);

    while (true) {
        server.connect(new Connection(*server.getSocket()->getSocket()));
        server.clientCommunication();
    }

    // not strictly necessary, but it's nice to be explicit about what we're doing
    close(*server.getSocket()->getSocket());
    return 0;
}
