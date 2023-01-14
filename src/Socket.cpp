#include "Socket.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 10 // how many pending connections queue will hold

/**
 * Use the code from the example to set up sockets for the client and the server.
 * Doing it in an object-oriented way makes the maintenance of the program simpler.
 * */

ClientSocket::ClientSocket(int port, char *ipAddress) {
    this->port = port;

    struct sockaddr_in server;
    // Create a new clientsocket
    if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Assign address family, port and IP address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_aton(ipAddress, &server.sin_addr);

    // Connect to server
    if (connect(this->sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connect error - no server available");
        exit(EXIT_FAILURE);
    }
}

ServerSocket::ServerSocket(int port) {
    this->port = port;

    struct sockaddr_in server_address;

    // Create a new serversocket
    if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &this->reuseValue,
                   sizeof(this->reuseValue)) == -1) {
        perror("set socket options - reuseAddr");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEPORT, &this->reuseValue,
                   sizeof(this->reuseValue)) == -1) {
        perror("set socket options - reusePort");
        exit(EXIT_FAILURE);
    }

    // Assign address family, port and any IP address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(this->sock, (struct sockaddr *)&server_address,
             sizeof(server_address)) == -1) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    // Listen for a connection
    if (listen(this->sock, BACKLOG) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening...\n";
}

int *Socket::getSocket() {
    return &sock;
}

void Socket::closeSocket() {
    close(this->sock);
}
