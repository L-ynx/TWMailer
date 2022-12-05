#ifndef CONNECTION_H_
#define CONNECTION_H_
#include "ServerSocket.hpp"

class Connection {
  private:
    int clientSocket = -1;

  public:
    Connection(int serverSocket);

    int getClientSocket();
};

#endif // CONNECTION_H_