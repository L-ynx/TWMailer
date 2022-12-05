#include "Connection.hpp"
#include "ServerSocket.hpp"
#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int abortRequested = 0;
int s0 = -1;
int s1 = -1;

void clientCommunication(int data);
void signalHandler(int sig);

static void usage() {
    std::cout
        << "Usage Server:\n\t./twmailer-server <port> <mail-spool-directoryname>\n";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usage();
        return EXIT_FAILURE;
    }

    ServerSocket serverSocket(atoi(argv[1]));
    Connection connection(serverSocket.getServerSocket());
    clientCommunication(connection.getClientSocket());

    return 0;
}

void clientCommunication(int data) {

    char buffer[4096];
    int message;
    int client = data;

    do {
        memset(buffer, 0, 4096);
        message = recv(client, buffer, 4096, 0);
        if (message == -1) {
            if (abortRequested) {
                perror("recv error after aborted");
            } else {
                perror("recv error");
            }
            break;
        }
        std::cout << std::string(buffer, 0, message) << std::endl;

        send(client, buffer, message + 1, 0);

    } while (strcmp(buffer, "QUIT") != 0 && !abortRequested);

    close(client);
}

void signalHandler(int sig) {
    if (sig == SIGINT) {
        printf("abort Requested... ");
        abortRequested = 1;

        if (s1 != -1) {
            if (shutdown(s1, SHUT_RDWR) == -1) {
                perror("shutdown s1");
            }
            if (close(s1) == -1) {
                perror("close s1");
            }
            s1 = -1;
        }

        if (s0 != -1) {
            if (shutdown(s0, SHUT_RDWR) == -1) {
                perror("shutdown s0");
            }
            if (close(s0) == -1) {
                perror("close s0");
            }
            s0 = -1;
        }
    } else {
        exit(sig);
    }
}
