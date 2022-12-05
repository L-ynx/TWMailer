#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

class ServerSocket {
  private:
    int port;
    int serverAddress = -1;

  public:
    ServerSocket(int port);

    int getServerAddress();
};

#endif // SERVER_SOCKET_H_
