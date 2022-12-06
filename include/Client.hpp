#ifndef CLIENT_H_
#define CLIENT_H_
#include "Socket.hpp"
#include <iostream>

class Client {
  private:
    int port;

    ClientSocket *clientSocket;

  public:
    Client(int port, char *ipAddress);

    ClientSocket *getSocket();

    static void usage() {
        std::cout << "Usage Client:\n\t./twmailer-client <ip> <port>\n";
    };
};

#endif // CLIENT_H_