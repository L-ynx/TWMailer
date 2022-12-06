#include "Connection.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port) {
    this->port = port;
    this->serverSocket = new ServerSocket(port);
    this->connection = new Connection(*serverSocket->getSocket());
}

Connection *Server::getConnection() {
    return this->connection;
}

ServerSocket *Server::getSocket() {
    return this->serverSocket;
}

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
        std::cout << std::string(buffer, 0, message) << std::endl;

        send(*this->connection->getClientSocket(), buffer, message + 1, 0);

    } while (strcasecmp(buffer, "QUIT") != 0 && !this->abortRequested);
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

    Server server(atoi(argv[1]));

    server.clientCommunication();
    close(*server.getSocket()->getSocket());
    return 0;
}
