#ifndef SOCKET_H_
#define SOCKET_H_

class Socket {
  protected:
    int port;
    int sock = -1;
    int reuseValue = 1;

  public:
    int *getSocket();
    void closeSocket();
};

class ServerSocket : public Socket {
  public:
    ServerSocket(int port);
};

class ClientSocket : public Socket {
  public:
    ClientSocket(int port, char *ipAddress);
};

#endif // SOCKET_H_
