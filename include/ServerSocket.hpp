#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

class ServerSocket {
  private:
    int port;
    int serverSocket = -1;
    int reuseValue = 1;

  public:
    ServerSocket(int port);

    int *getServerSocket();
    void closeServerSocket();
};

#endif // SERVER_SOCKET_H_
