#ifndef SERVER_H_
#define SERVER_H_
#include "Connection.hpp"
#include "ServerSocket.hpp"
#include <iostream>

class Server {
  private:
    int port;
    int abortRequested = 0;

    Connection *connection;
    ServerSocket *serverSocket;

  public:
    Server(int port);

    void clientCommunication();
    void signalHandler(int sig);

    static void usage() {
        std::cout << "Usage Server:\n\t./twmailer-server <port> "
                     "<mail-spool-directoryname>\n";
    };
};

#endif // SERVER_H_
