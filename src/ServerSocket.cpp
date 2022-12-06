#include "ServerSocket.hpp"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

ServerSocket::ServerSocket(int port) {
    this->port = port;

    struct sockaddr_in server_address;

    if ((this->serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

   if (setsockopt(this->serverSocket,
                  SOL_SOCKET,
                  SO_REUSEADDR,
                  &this->reuseValue,
                  sizeof(this->reuseValue)) == -1)
   {
      perror("set socket options - reuseAddr");
      exit(EXIT_FAILURE);
   }

   if (setsockopt(this->serverSocket,
                  SOL_SOCKET,
                  SO_REUSEPORT,
                  &this->reuseValue,
                  sizeof(this->reuseValue)) == -1)
   {
      perror("set socket options - reusePort");
      exit(EXIT_FAILURE);
   }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->serverSocket, (struct sockaddr *)&server_address,
             sizeof(server_address)) == -1) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if (listen(this->serverSocket, 1) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
}

int *ServerSocket::getServerSocket() {
    return &serverSocket;
}

void ServerSocket::closeServerSocket() {
    close(this->serverSocket);
}
