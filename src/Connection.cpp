#include "Connection.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * Use the code from the example to set up a connection. Doing it in an
 * object-oriented way makes the maintenance of the program simpler.
 * */
Connection::Connection(int serverSocket) {
    socklen_t clientSize;
    struct sockaddr_in client_address;

    clientSize = sizeof(client_address);
    // block until client connects to server
    if ((this->clientSocket = accept(
             serverSocket, (struct sockaddr *)&client_address, &clientSize)) == -1) {
        perror("accept error");
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    memset(host, 0, NI_MAXHOST);
    memset(serv, 0, NI_MAXSERV);

    // Print connection info
    int res = getnameinfo((sockaddr *)&client_address, sizeof(client_address), host,
                          NI_MAXHOST, serv, NI_MAXSERV, 0);
    if (res) {
        std::cout << host << " connected on " << serv << std::endl;
    } else {
        inet_ntop(AF_INET, &client_address.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client_address.sin_port)
                  << std::endl;
    }
    setClientAddress(host);
}

int *Connection::getClientSocket() {
    return &this->clientSocket;
}

std::string Connection::getClientAddress() {
    return this->clientAddress;
}

void Connection::setClientAddress(std::string client_address) {
    this->clientAddress = client_address;
}
