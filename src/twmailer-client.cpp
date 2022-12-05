#include <arpa/inet.h>
#include <climits>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static void usage();
static void commands();
static void help();
std::string sendMsg();
std::string listMsg();
std::string readOrDelMsg();

int main(int argc, char *argv[]) {
    struct sockaddr_in server;
    int port;
    char buffer[4096];

    if (argc != 3) {
        usage();
        exit(1);
    }

    // Create a client socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket error.");
        return EXIT_FAILURE;
    }

    port = atoi(argv[2]);

    // Assign protocol family, port and address of server
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_aton(argv[1], &server.sin_addr);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // Print available commands
    commands();

    std::string input;
    // Send and receive data
    do {
        bool validCmd = false;
        std::string data;

        // Enter valid command
        while (!validCmd) {
            data = "";
            std::cout << "> ";
            getline(std::cin, input);
            data += input;

            if (strcasecmp(data.c_str(), "QUIT") == 0) {
                if (sock != -1) {
                    if (shutdown(sock, SHUT_RDWR) == -1) {
                        perror("shutdown sock");
                    }
                    if (close(sock) == -1) {
                        perror("close sock");
                    }
                    sock = -1;
                }
                return EXIT_SUCCESS;
            }

            if (strcasecmp(data.c_str(), "SEND") == 0) {
                data += sendMsg();
                validCmd = true;
            } else if (strcasecmp(data.c_str(), "LIST") == 0) {
                data += listMsg();
                validCmd = true;
            } else if (strcasecmp(data.c_str(), "READ") == 0 ||
                       strcasecmp(data.c_str(), "DEL") == 0) {
                data += readOrDelMsg();
                validCmd = true;
            } else if (strcasecmp(data.c_str(), "HELP") == 0) {
                help();
            } else
                continue;

            // Ignore last newline to empty getline buffer
            std::cin.ignore(INT_MAX, '\n');
        }

        // Send Client input data to server
        if ((send(sock, data.c_str(), data.size() + 1, 0)) == -1) {
            perror(
                "An error occurred while trying to send the data to the server.\n");
            continue;
        }
        // Receive answer from server
        memset(buffer, 0, 4096);
        int feedback = recv(sock, buffer, 4096, 0);

        if (feedback == -1) {
            perror("An error occurred while trying to receive the data from the "
                   "server.\n");
            break;
        } else if (feedback == 0) {
            std::cout << "Server closed remote socket\n";
            break;
        } else {
            std::cout << std::string(buffer, feedback) << std::endl;
        }

    } while (true);

    close(sock);

    return EXIT_SUCCESS;
}

std::string sendMsg() {
    std::string sender, receiver, subject, msg, msgLine;

    std::cout << "\nSender: ";
    std::cin >> sender;
    std::cout << "Receiver: ";
    std::cin >> receiver;
    std::cout << "Subject: ";
    std::cin >> subject;
    std::cout << "Message: ";

    while (msgLine != ".") {
        std::cin >> msgLine;
        msg += msgLine + "\n";
    }

    if (sender.size() > 8)
        sender.resize(8);
    if (receiver.size() > 8)
        receiver.resize(8);
    if (subject.size() > 80)
        subject.resize(80);

    return "\n" + sender + "\n" + receiver + "\n" + subject + "\n" + msg + "\n";
}

std::string listMsg() {
    std::string username;

    std::cout << "\nUsername: ";
    std::cin >> username;

    return "\n" + username + "\n";
}

std::string readOrDelMsg() {
    std::string username, msgNr;
    std::cout << "\nUsername: ";
    std::cin >> username;
    std::cout << "Message-Number: ";
    std::cin >> msgNr;

    return "\n" + username + "\n" + msgNr + "\n";
}

static void usage() {
    std::cout << "Usage Client:\n\t./twmailer-client <ip> <port>\n";
}

static void commands() {
    std::cout << "Available commands:\tSEND\tLIST\tREAD\tDEL\tQUIT\tHELP\n";
}

static void help() {
    char c;

    std::cout << "(S)END: client sends a message to the server.\n(L)IST: lists all "
                 "messages of a specific user.\n(R)EAD: display a specific message "
                 "of a specific user.\n(D)EL: removes a specific message.\n(Q)UIT: "
                 "logout the client.\n";
    std::cout << "For further information, enter the desired key. Enter any other "
                 "key to return\n";

    while (true) {
        std::cin >> c;
        c = toupper(c);

        switch (c) {
        case 'S':
            std::cout << "\nSEND\n\t<Sender>\n\t<Receiver>\n\t<Subject (max. 80 "
                         "chars)>\n\t<message (multi-line; no length "
                         "restrictions)>\n\t.\n";
            continue;
        case 'L':
            std::cout << "\nLIST\n\t<Username>\n";
            continue;
        case 'R':
            std::cout << "\nREAD\n\t<Username>\n\t<Message-Number>\n";
            continue;
        case 'D':
            std::cout << "\nDEL\n\tUsername>\n\t<Message-Number>\n";
            continue;
        case 'Q':
            std::cout << "\nQUIT\n";
            continue;
        default:
            break;
        }
        break;
    }
}