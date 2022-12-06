#ifndef SERVER_H_
#define SERVER_H_
#include "CommandHandler.hpp"
#include "Connection.hpp"
#include "ServerSocket.hpp"
#include <iostream>

class Server {
  private:
    int port;
    int abortRequested = 0;
    std::string maildir;

    Connection *connection;
    ServerSocket *serverSocket;

    CommandHandler *ch;

  public:
    Server(int port, std::string directory);

    void connect(Connection *client);

    Connection *getConnection();
    ServerSocket *getServerSocket();

    void clientCommunication();
    void signalHandler(int sig);

    static void usage() {
        std::cout << "Usage Server:\n\t./twmailer-server <port> "
                     "<mail-spool-directoryname>\n";
    };
};

#endif // SERVER_H_
