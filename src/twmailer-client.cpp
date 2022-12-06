#include "Client.hpp"
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
std::string getUserInput();

Client::Client(int port, char *ipAddress) {
    this->port = port;
    this->clientSocket = new ClientSocket(port, ipAddress);
}

ClientSocket *Client::getSocket() {
    return this->clientSocket;
}

int main(int argc, char *argv[]) {
    int isQuit;
    char buffer[4096];

    if (argc != 3) {
        usage();
        exit(1);
    }

    Client client(atoi(argv[2]), argv[1]);

    // Print available commands
    commands();

    // Send and receive data
    do {
        std::string data = getUserInput();
        isQuit = strcasecmp(data.c_str(), "QUIT") == 0;

        // Send Client input data to server
        if ((send(*client.getSocket()->getSocket(), data.c_str(), data.size() + 1,
                  0)) == -1) {
            perror("An error occurred while trying to send data to the server.\n");
            continue;
        }
        // Receive answer from server
        memset(buffer, 0, 4096);
        int feedback = recv(*client.getSocket()->getSocket(), buffer, 4096, 0);

        if (feedback == -1) {
            perror("An error occurred while trying to receive data from the "
                   "server.\n");
            break;
        } else if (feedback == 0) {
            std::cout << "Server closed remote socket\n";
            break;
        } else {
            std::cout << std::string(buffer, feedback) << std::endl;
        }

    } while (!isQuit);

    if (*client.getSocket()->getSocket() != -1) {
        if (shutdown(*client.getSocket()->getSocket(), SHUT_RDWR) == -1) {
            // invalid in case the server is gone already
            perror("Shutdown *client.getSocket()->getSocket()");
        }
        if (close(*client.getSocket()->getSocket()) == -1) {
            perror("Close *client.getSocket()->getSocket()");
        }
        *client.getSocket()->getSocket() = -1;
    }

    return EXIT_SUCCESS;
}

// Read User command and message
std::string getUserInput() {
    bool validCmd = false;
    std::string data, input;
    while (!validCmd) {
        data = "";
        std::cout << "> ";
        // Get Usercommand and convert to upper case
        getline(std::cin, input);
        for (auto &c : input)
            data += toupper(c);

        if (data == "QUIT") {
            break;
        } else if (data == "SEND") {
            data += sendMsg();
            validCmd = true;
        } else if (data == "LIST") {
            data += listMsg();
            validCmd = true;
        } else if (data == "READ" || data == "DEL") {
            data += readOrDelMsg();
            validCmd = true;
        } else if (data == "HELP") {
            help();
        } else
            continue;

        // Ignore last newline to empty getline buffer
        // std::cin.ignore(INT_MAX, '\n');
        // has the same effect without needing to press
        // enter again to make it work
        std::cin.clear();
    }
    return data;
}

std::string sendMsg() {
    std::string sender, receiver, subject, msg, msgLine;

    std::cout << "\nSender: ";
    getline(std::cin, sender);
    std::cout << "Receiver: ";
    getline(std::cin, receiver);
    std::cout << "Subject: ";
    getline(std::cin, subject);
    std::cout << "Message: ";

    while (msgLine != ".") {
        getline(std::cin, msgLine);
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
