#include "Client.hpp"
#include <arpa/inet.h>
#include <climits>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

static void usage();

Client::Client(int port, char *ipAddress) {
    this->port = port;
    this->clientSocket = new ClientSocket(port, ipAddress);
}

ClientSocket *Client::getSocket() {
    return this->clientSocket;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        usage();
        exit(1);
    }

    // Create a new client
    Client client(atoi(argv[2]), argv[1]);
    client.clientCommunication();

    return EXIT_SUCCESS;
}

void Client::clientCommunication() {

    char buffer[4096];
    std::string msg;
    this->commands();

    // Send and receive data
    do {
        std::string data = this->getUserInput();
        isQuit = strcasecmp(data.c_str(), "QUIT") == 0;
        if (isQuit)
            break;

        // Send Client input data to server
        if ((send(*this->getSocket()->getSocket(), data.c_str(), data.size() + 1,
                  0)) == -1) {
            perror("An error occurred while trying to send data to the server.\n");
            continue;
        }
        // Receive answer from server
        memset(buffer, 0, 4096);
        int feedback = recv(*this->getSocket()->getSocket(), buffer, 4096, 0);

        if (feedback == -1) {
            perror("An error occurred while trying to receive data from the "
                   "server.\n");
            break;
        } else if (feedback == 0) {
            std::cout << "Server closed remote socket\n";
            break;
        } else {
            msg = std::string(buffer, feedback);
            std::cout << std::string(buffer, feedback) << std::endl;

            if (loginAttempt > 0) {
                if (strcasecmp(msg.c_str(), "OK") == 0) {
                    loginAttempt = 0;
                    isLoggedIn = true;

                    std::cout << "\nLogin successful!\n\n";
                } else {
                    std::cout << "\nLogin failed!\n";
                }
            }
            this->commands();
        }

    } while (!isQuit);

    // Close socket
    if (*this->getSocket()->getSocket() != -1) {
        if (shutdown(*this->getSocket()->getSocket(), SHUT_RDWR) == -1) {
            // invalid in case the server is gone already
            perror("Shutdown this->getSocket()->getSocket()");
        }
        if (close(*this->getSocket()->getSocket()) == -1) {
            perror("Close this->getSocket()->getSocket()");
        }
        *this->getSocket()->getSocket() = -1;
    }
}

// Read User command and message
std::string Client::getUserInput() {
    bool validCmd = false;
    bool send = false;
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
        }
        if (isLoggedIn) {
            if (data == "SEND") {
                data += this->sendMsg();
                send = true;
                validCmd = true;
            } else if (data == "LIST") {
                data += this->listMsg();
                validCmd = true;
            } else if (data == "READ" || data == "DEL") {
                data += this->readOrDelMsg();
                validCmd = true;
            } else if (data == "HELP") {
                help();
            }
        } else if (!isLoggedIn && data == "LOGIN") {
            data += this->login();
            send = true;
            validCmd = true;
        } else
            continue;

        // Ignore last newline to empty getline buffer
        if (!send) {
            std::cin.ignore(INT_MAX, '\n');
        }
    }
    return data;
}

std::string Client::sendMsg() {
    std::string receiver, subject, msg, msgLine;

    std::cout << "Receiver: ";
    getline(std::cin, receiver);
    std::cout << "Subject: ";
    getline(std::cin, subject);
    std::cout << "Message: ";

    while (msgLine != ".") {
        getline(std::cin, msgLine);
        msg += msgLine + "\n";
    }

    if (receiver.size() > 8)
        receiver.resize(8);
    if (subject.size() > 80)
        subject.resize(80);

    return "\n" + this->user + "\n" + receiver + "\n" + subject + "\n" + msg + "\n";
}

std::string Client::listMsg() {
    return "\n" + this->user + "\n";
}

std::string Client::readOrDelMsg() {
    std::string msgNr;
    std::cout << "Message-Number: ";
    std::cin >> msgNr;

    return "\n" + this->user + "\n" + msgNr + "\n";
}

std::string Client::login() {
    std::string username, password;
    std::cout << "\nPlease enter your credentials. The username has a maximum "
                 "length of 8 chars. Automatic trim in case of Overflow\n";

    while (password.size() == 0) {
        std::cout << "\nUsername: ";
        getline(std::cin, username);
        std::cout << "\nPassword: ";
        password = getPW();
        if (password.size() == 0) {
            std::cout << "\nPassword can't be empty!";
        }
    }

    if (username.size() > 8)
        username.resize(8);

    this->user = username;
    this->loginAttempt++;

    return "\n" + this->user + "\n" + password + "\n";
}

int Client::getch() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

// Read Password Input and hide with asterisks
std::string Client::getPW() {
    const char BACKSPACE = 127;
    const char RETURN = 10;

    std::string password;
    unsigned char ch = 0;

    while ((ch = getch()) != RETURN) {
        if (ch == BACKSPACE) {
            if (password.length() != 0) {
                std::cout << "\b \b";
                password.resize(password.length() - 1);
            }
        } else {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

static void usage() {
    std::cout << "Usage Client:\n\t./twmailer-client <ip> <port>\n";
}

void Client::commands() {
    if (!isLoggedIn)
        std::cout << "Available commands: \tLOGIN\tQUIT\n";
    else
        std::cout << "Available commands:\tSEND\tLIST\tREAD\tDEL\tQUIT\tHELP\n";
}

void Client::help() {
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
