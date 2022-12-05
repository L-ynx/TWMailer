#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

int abortRequested = 0;
int s0 = -1;
int s1 = -1;

void clientCommunication(void *data); void signalHandler(int sig);

static void usage() {
    std::cout << "Usage Server:\n\t./twmailer-server <port> <mail-spool-directoryname>";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        usage();
        exit(1);
    }

    socklen_t clientSize;
    struct sockaddr_in server_address, client_address;
    int port = atoi(argv[1]);
    int s0, s1;

    if ((s0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(s0, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind error");
        return EXIT_FAILURE;
    }

    if (listen(s0, 1) == -1) {
      perror("listen error");
      return EXIT_FAILURE;
   }

    clientSize = sizeof(client_address);
    if ((s1 = accept(s0, (struct sockaddr *) &client_address, &clientSize)) == -1) {
        perror("accept error");
    }

    close(s0);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    memset(host, 0, NI_MAXHOST);
    memset(serv, 0, NI_MAXSERV);


    int res = getnameinfo((sockaddr*)&client_address, sizeof(client_address), host, NI_MAXHOST, serv, NI_MAXSERV, 0);
    if(res) {
        std::cout << host << " connected on " << serv << std::endl;
    } else {
        inet_ntop(AF_INET, &client_address.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client_address.sin_port) << std::endl;
    }
    clientCommunication(&s1);
    
    close(s1);
    return 0;
}

void clientCommunication(void *data) {
    
    char buffer[4096];
    int message;
    int *client = (int*) data;

    do {
        memset(buffer, 0, 4096);
        message = recv(*client, buffer, 4096, 0);
        if (message == -1) {
            if (abortRequested) {
                perror("recv error after aborted");
            } else  {
                perror("recv error");
            }
            break;
        }
        std::cout << std::string(buffer, 0, message) << std::endl;

        send(*client, buffer, message+1, 0);


    } while(strcmp(buffer, "QUIT") != 0 && !abortRequested);
    
    close(*client);
}

void signalHandler(int sig)
{
   if (sig == SIGINT) {
      printf("abort Requested... ");
      abortRequested = 1;

      if (s1 != -1)  {
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
   }
   else {
      exit(sig);
   }
}