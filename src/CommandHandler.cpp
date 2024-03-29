#include "CommandHandler.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ldap.h>
#include <mutex>
#include <sstream>
#include <string>
#include <unistd.h>

CommandHandler::CommandHandler(std::string directory) {
    this->maildir = directory;
    createDirectory(this->maildir);
}

void CommandHandler::createDirectory(std::string directory) {
    try {
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directory(directory);
        }
    } catch (std::filesystem::filesystem_error &error) {
        std::cerr << error.what() << '\n';
    }
}

void CommandHandler::parseInput(std::string input) {
    input = readCommand(input);
    input = readSender(input);
    if (this->command == "SEND") {
        saveMessage(input);
    } else if (this->command == "LIST") {
        listMessages(input);
    } else if (this->command == "READ") {
        readMessage(input);
    } else if (this->command == "DEL") {
        deleteMessage(input);
    } else if (this->command == "LOGIN") {
        attemptLogin(input);
    } else {
        setResponse("ERR");
    }
}

void CommandHandler::attemptLogin(std::string input) {
    /* The logic here may seem confusing at first glance, but it's fairly
          simple: the user has failed to log in enough times, they will be
          blacklisted. We need to check this with each new attempt because we
          don't want to bother with any of the rest of the code in this method
          if they haven't waited long enough.  If they have actually waited long
          enough, we want to disable the failure check and try to log them in
          (so that they can fail again if they want).
    */
    if (this->failedAttempts >= ATTEMPT_LIMIT) {
        if (!isBlacklisted()) {
            this->failedAttempts = 0;
            this->attemptLogin(input);
        } else {
            setResponse("Too many incorrect login attempts.\nTry again later.");
        }
    } else {
        // LDAP config
        // anonymous bind with user and pw empty
        const char *ldapUri = "ldap://ldap.technikum-wien.at:389";
        const int ldapVersion = LDAP_VERSION3;
        int rc = 0; // return code
        std::string password = trimPW(input);
        std::string username =
            "uid=" + this->sender + ",ou=people,dc=technikum-wien,dc=at";

        // setup LDAP connection
        LDAP *ldapHandle;
        rc = ldap_initialize(&ldapHandle, ldapUri);

        if (rc != LDAP_SUCCESS) {
            fprintf(stderr, "ldap_init failed\n");
            setResponse("ERR\n");
            return;
        }

        // set version options
        rc = ldap_set_option(ldapHandle,
                             LDAP_OPT_PROTOCOL_VERSION, // OPTION
                             &ldapVersion);             // IN-Value
        if (rc != LDAP_OPT_SUCCESS) {
            fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n",
                    ldap_err2string(rc));
            ldap_unbind_ext_s(ldapHandle, NULL, NULL);
            setResponse("ERR\n");
            return;
        }

        // initialize TLS
        rc = ldap_start_tls_s(ldapHandle, NULL, NULL);
        if (rc != LDAP_SUCCESS) {
            fprintf(stderr, "ldap_start_tls_s(): %s\n", ldap_err2string(rc));
            ldap_unbind_ext_s(ldapHandle, NULL, NULL);
            setResponse("ERR\n");
            return;
        }

        // bind credentials
        BerValue bindCredentials;
        bindCredentials.bv_val = (char *)password.c_str();
        bindCredentials.bv_len = strlen(password.c_str());
        BerValue *servercredp; // server's credentials
        rc = ldap_sasl_bind_s(ldapHandle, username.c_str(), LDAP_SASL_SIMPLE,
                              &bindCredentials, NULL, NULL, &servercredp);
        if (rc != LDAP_SUCCESS) {
            fprintf(stderr, "LDAP bind error: %s\n", ldap_err2string(rc));
            ldap_unbind_ext_s(ldapHandle, NULL, NULL);
            setResponse("ERR\n");
            if (++this->failedAttempts >= this->ATTEMPT_LIMIT) {
                blacklistSender();
            }
            return;
        }
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);

        setResponse("OK\n");
    }
}

void CommandHandler::saveMessage(std::string input) {
    std::stringstream istringstream(input);
    // Create a directory for the sender
    std::string senderDirectory = this->maildir + "/" + this->sender;
    createDirectory(senderDirectory);

    // Create a new file for each message
    std::ofstream of{senderDirectory + "/" +
                     std::to_string(nextFreeFileNumber(senderDirectory)) + ".msg"};
    if (!of) {
        setResponse("ERR\n");
        return;
    }

    of << sender << '\n';

    // Write to filelist
    while (istringstream) {
        std::string segment;
        getline(istringstream, segment);
        of << segment << '\n';
        if (segment == ".") {
            break;
        }
    }

    setResponse("OK\n");
    of.close();
}

void CommandHandler::listMessages(std::string input) {
    std::stringstream istringstream(input);

    std::string senderDirectory = this->maildir + "/" + this->sender;
    std::string response;

    if (!std::filesystem::exists(senderDirectory)) {
        response = "0";
    } else if (numberOfFiles(senderDirectory) == 0) {
        response = "0";
    } else {
        // Count amount of messages sent by user and get receiver names
        response += std::to_string(numberOfFiles(senderDirectory)) + "\n";
        std::filesystem::path path = senderDirectory;
        for (auto &file : std::filesystem::directory_iterator(path)) {
            std::ifstream is{file.path()};
            std::string receiver, subject;
            getline(is, receiver);
            getline(is, subject);
            // this line is not strictly the same as the specification, but it
            // makes the user's life easier to see what the filename is when
            // they list their messages. This way they know what number they
            // need to use in order to read it later.
            response += file.path().filename().string() + ": " + subject + "\n";
            is.close();
        }
    }
    setResponse(response);
}

void CommandHandler::readMessage(std::string input) {
    input = readMessageNumber(input);
    std::stringstream istringstream(input);

    std::string senderDirectory = this->maildir + "/" + this->sender;
    std::string filename = senderDirectory + "/" + this->messageNumber + ".msg";
    std::string response;

    if (!std::filesystem::exists(senderDirectory) ||
        !numberOfFiles(senderDirectory) ||
        !std::filesystem::exists(std::filesystem::directory_entry(filename))) {
        response = "ERR\n";
    } else {
        response = "OK\n";
        std::ifstream is{filename};
        std::string content;
        while (is) {
            getline(is, content);
            response += content + "\n";
        }
        is.close();
    }
    setResponse(response);
}

void CommandHandler::deleteMessage(std::string input) {
    // Critical section: shared resource is accessed here, lock is automatically
    // released when lock goes out of scope
    std::lock_guard<std::mutex> guard(this->_mutex);
    input = readMessageNumber(input);
    std::stringstream istringstream(input);

    std::string senderDirectory = this->maildir + "/" + this->sender;
    std::string filename = senderDirectory + "/" + messageNumber + ".msg";
    std::string response;

    if (!std::filesystem::exists(senderDirectory) ||
        !numberOfFiles(senderDirectory) ||
        !std::filesystem::exists(std::filesystem::directory_entry(filename))) {
        response = "ERR\n";
    } else {
        response = "OK\n";
        std::filesystem::remove(filename);
    }
    setResponse(response);
}

std::string CommandHandler::readCommand(std::string input) {
    std::stringstream istringstream(input);
    getline(istringstream, this->command);
    return trim(input);
}

std::string CommandHandler::readSender(std::string input) {
    std::stringstream istringstream(input);
    getline(istringstream, this->sender);
    return trim(input);
}

std::string CommandHandler::readMessageNumber(std::string input) {
    std::stringstream istringstream(input);
    getline(istringstream, this->messageNumber);
    return trim(input);
}

std::string CommandHandler::trim(std::string input) {
    std::string message = "";
    std::stringstream istringstream(input);
    getline(istringstream, this->messageNumber);
    while (istringstream) {
        std::string line;
        getline(istringstream, line);
        message += line + "\n";
    }
    return message;
}

std::string CommandHandler::trimPW(std::string input) {
    if (!input.empty()) {
        while (input.back() == '\n')
            input.pop_back();
    }
    return input;
}

int CommandHandler::numberOfFiles(std::string directory) {
    int counter = 0;
    std::filesystem::path path = directory;
    for (auto &file : std::filesystem::directory_iterator(path)) {
        std::cout << "[DEBUG INFO] " << file.path() << '\n';
        counter++;
    }
    return counter;
}

int CommandHandler::nextFreeFileNumber(std::string directory) {
    std::lock_guard<std::mutex> guard(this->_mutex);
    std::filesystem::directory_entry entry;
    int i = 0;
    while (true) {
        entry = std::filesystem::directory_entry(directory + "/" +
                                                 std::to_string(i) + ".msg");
        if (!std::filesystem::exists(entry)) {
            return i;
        }
        i++;
    }
}

void CommandHandler::setSenderIP(std::string ip) {
    this->senderIP = ip;
}

void CommandHandler::setResponse(std::string message) {
    this->response = message.data();
    this->responseLength = message.length();
}

std::string CommandHandler::getResponse() {
    return this->response;
}
int CommandHandler::getResponseLength() {
    return this->responseLength;
}

// Returns true if the user was blacklisted less than 60 seconds ago.
bool CommandHandler::isBlacklisted() {
    readBlacklist();
    return (this->blacklist[this->senderIP] > (int)time(0) - this->DELAY);
}

void CommandHandler::blacklistSender() {
    readBlacklist();
    this->blacklist[this->senderIP] = (int)time(0);
    saveBlacklist();
}

void CommandHandler::readBlacklist() {
    std::lock_guard<std::mutex> guard(this->_mutex);
    std::string file = "blacklist";

    if (!std::filesystem::exists(std::filesystem::directory_entry(file))) {
        return;
    } else {
        std::ifstream inf{file};

        while (inf) {
            std::string inputLine;
            std::getline(inf, inputLine);

            if (inputLine != "") {
                std::istringstream iss(inputLine);
                std::string address;
                std::string timeString;
                int time;
                iss >> address;
                iss >> timeString;
                time = atoi(timeString.c_str());
                blacklist[address] = time;
            }
        }
        for (const auto &[key, value] : this->blacklist) {
            std::cout << key << " blacklisted at " << value << std::endl;
        }
    }
}

void CommandHandler::saveBlacklist() {
    std::lock_guard<std::mutex> guard(this->_mutex);
    std::string file = "blacklist";

    std::ofstream of{file};
    if (!of) {
        setResponse("ERR\n");
        return;
    }

    for (const auto &[key, value] : this->blacklist) {
        std::cout << key << " blacklisted at " << value << std::endl;
        of << key << ' ' << value << '\n';
    }

    of.close();
}
