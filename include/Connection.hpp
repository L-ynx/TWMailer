#ifndef CONNECTION_H_
#define CONNECTION_H_
#include "Socket.hpp"
#include <string>

class Connection {
  private:
    int clientSocket = -1;
    std::string clientAddress;

  public:
    Connection(int serverSocket);

    int *getClientSocket();
    std::string getClientAddress();
    void setClientAddress(std::string client_address);
};

#endif // CONNECTION_H_
