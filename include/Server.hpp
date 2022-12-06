#ifndef SERVER_H_
#define SERVER_H_
#include "Connection.hpp"
#include "ServerSocket.hpp"

class Server {
  private:
    int port;

    Connection *connection;
    ServerSocket *serverSocket;

  public:
    Server(int port);
    Connection getConnection();

    void clientCommunication();
};

#endif // SERVER_H_
