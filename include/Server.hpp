#ifndef SERVER_H_
#define SERVER_H_
#include "CommandHandler.hpp"
#include "Connection.hpp"
#include "Socket.hpp"
#include <iostream>
#include <vector>

class Server {
  private:
    int port;
    int abortRequested = 0;
    std::string maildir;

    std::vector<Connection *> connection;
    ServerSocket *serverSocket;

    CommandHandler *ch;

  public:
    Server(int port, std::string directory);

    void connect(Connection *client);

    Connection *getConnection();
    ServerSocket *getSocket();

    void clientCommunication(Connection *conn);
    void signalHandler(int sig);

    static void usage() {
        std::cout << "Usage Server:\n\t./twmailer-server <port> "
                     "<mail-spool-directoryname>\n";
    };
};

#endif // SERVER_H_
