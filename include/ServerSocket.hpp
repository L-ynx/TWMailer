#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

class ServerSocket {
  private:
    int port;
    int serverSocket = -1;

  public:
    ServerSocket(int port);

    int *getServerSocket();
};

#endif // SERVER_SOCKET_H_
