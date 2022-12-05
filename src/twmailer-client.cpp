#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

static void usage() {
    std::cout << "Usage Client:\n\t./twmailer-client <ip> <port>\n";
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server;
    int port;
    char buffer[4096];

    if (argc != 3) {
        usage();
        exit(1);
    }

    int s0 = socket(AF_INET, SOCK_STREAM, 0);
    if (s0 < 0) {
        perror("Socket error.");
        return EXIT_FAILURE;
    }

    port = atoi(argv[2]);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    // Deprecated
    inet_aton(argv[1], &server.sin_addr);
    std::cout << "JASDASD";
    if (connect(s0, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    std::string input;

    do {
        std::cout << "> ";
        getline(std::cin, input);

        if ((send(s0, input.c_str(), input.size() + 1, 0)) == -1) {
            perror("Input Error");
            continue;
        }
        memset(buffer, 0, 4096);
        int feedback = recv(s0, buffer, 4096, 0);

        if (feedback == -1) {
            perror("recv error");
            break;
        }
        else if (feedback == 0) {
            printf("Server closed remote socket\n");
            break;
        }
        else {
            std::cout << std::string(buffer, feedback) << std::endl;
        }

    } while (true);

    close(s0);

    std::cout << "Hello, World from Client!" << std::endl;
    return 0;
}
