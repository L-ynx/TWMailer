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

static void commands() {
    std::cout << "Available commands:\tSEND\tLIST\tREAD\tDEL\tQUIT\tHELP\n";
}

static void help() {
    char c;

    std::cout << "(S)END: client sends a message to the server.\n(L)IST: lists all messages of a specific user.\n(R)EAD: display a specific message of a specific user.\n(D)EL: removes a specific message.\n(Q)UIT: logout the client.\n";
    std::cout << "For further information, enter the desired key. Enter any other key to return\n";
    
    while (true) {
        std::cin >> c;
        c = toupper(c);

        switch (c) {
        case 'S': std::cout << "\nSEND\n\t<Sender>\n\t<Receiver>\n\t<Subject (max. 80 chars)>\n\t<message (multi-line; no length restrictions)>\n\t.\n";
            continue;
        case 'L': std::cout << "\nLIST\n\t<Username>\n";
            continue;
        case 'R': std::cout << "\nREAD\n\t<Username>\n\t<Message-Number>\n";
            continue;
        case 'D': std::cout << "\nDEL\n\tUsername>\n\t<Message-Number>\n";
            continue;
        case 'Q': std::cout << "\nQUIT\n";
            continue;
        default:
            break;
        }
        break;
    }
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

    if (connect(s0, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }


    commands();
    std::string input;

    do {
        int type = 0;
        int counter = 1;
        std::string data;

        while (type == 0) {
            data = "";
            std::cout << "> ";
            getline(std::cin, input);
            data += input;


            // Here we could possibly use a function pointer
            if (data == "SEND")
                type = 1;
            else if (data == "LIST")
                type = 2;
            else if (data == "READ" || data == "DEL") {
                type = 2;
                counter = 2;
            }
            else if (data == "HELP")
                type = 3;
            else if (data == "QUIT")
                type = 4;

            data += "\n";

            switch (type) {
            case 1: while (input != ".") {
                        getline(std::cin, input);
                        data += input + "\n";
                    }
                    break;

            case 2: for (int i = 0; i < counter; i++) {
                        getline(std::cin, input);
                        data += input + "\n";
                    }
                    break;
            case 3: help();     //Why does it print the default line, after exiting this function?
                    type = 0;
                    break;
            default: std::cout << "Please enter a valid command." << std::endl;
                     break;
            }
        }

        if ((send(s0, data.c_str(), data.size() + 1, 0)) == -1) {
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
