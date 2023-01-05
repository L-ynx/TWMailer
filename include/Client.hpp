#ifndef CLIENT_H_
#define CLIENT_H_
#include "Socket.hpp"
#include <iostream>

class Client {
  private:
    int port;
    int isQuit = 0;
    int loginAttempt = 0;
    bool isLoggedIn = false;
    std::string user;

    ClientSocket *clientSocket;

  public:
    Client(int port, char *ipAddress);

    ClientSocket *getSocket();

    void clientCommunication();
    void login();
    void commands();
    std::string loginInput();
    std::string readOrDelMsg();
    std::string listMsg();
    std::string sendMsg();
    std::string getUserInput();
    void help();
};

#endif // CLIENT_H_