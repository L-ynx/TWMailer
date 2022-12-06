#include "Connection.hpp"
#include "Server.hpp"
#include "ServerSocket.hpp"
#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int abortRequested = 0;
int s0 = -1;
int s1 = -1;

static void usage() {
    std::cout
        << "Usage Server:\n\t./twmailer-server <port> <mail-spool-directoryname>\n";
}

Server::Server(int port) {
    this->port = port;

    this->serverSocket = new ServerSocket(port);
    this->connection = new Connection(serverSocket->getServerSocket());
}

Connection Server::getConnection() {
    return *this->connection;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usage();
        return EXIT_FAILURE;
    }

    Server server(atoi(argv[1]));

    server.clientCommunication();
    return 0;
}

void Server::clientCommunication() {
    char buffer[4096];
    int message;

    do {
        memset(buffer, 0, 4096);
        message = recv(*this->connection->getClientSocket(), buffer, 4096, 0);
        if (message == -1) {
            if (abortRequested) {
                perror("recv error after aborted");
            } else {
                perror("recv error");
            }
            break;
        }
        std::cout << std::string(buffer, 0, message) << std::endl;

        send(*this->connection->getClientSocket(), buffer, message + 1, 0);

    } while (strcmp(buffer, "QUIT") != 0 && !abortRequested);

    close(*this->connection->getClientSocket());
}

void Server::signalHandler(int sig) {
    if (sig == SIGINT) {
        printf("abort Requested... ");
        abortRequested = 1;

        if (s1 != -1) {
            if (shutdown(s1, SHUT_RDWR) == -1) {
                perror("shutdown s1");
            }
            if (close(s1) == -1) {
                perror("close s1");
            }
            s1 = -1;
        }

        if (s0 != -1) {
            if (shutdown(s0, SHUT_RDWR) == -1) {
                perror("shutdown s0");
            }
            if (close(s0) == -1) {
                perror("close s0");
            }
            s0 = -1;
        }
    } else {
        exit(sig);
    }
}
