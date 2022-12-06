#include "CommandHandler.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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
    std::string segment;
    std::stringstream istringstream(input);
    getline(istringstream, segment, '\n');

    if (segment == "SEND") {
        saveMessage(input);
    } else if (segment == "LIST") {
    } else if (segment == "READ") {
    } else if (segment == "DEL") {
    }
}

void CommandHandler::saveMessage(std::string msg) {
    std::stringstream istringstream(msg);
    std::string segment;
    getline(istringstream, segment, '\n');

    std::string sender;
    getline(istringstream, sender, '\n');

    std::string receiver;
    getline(istringstream, receiver, '\n');

    std::string subject;
    getline(istringstream, subject, '\n');

    std::string senderDirectory = this->maildir + "/" + sender;
    createDirectory(senderDirectory);

    std::ofstream of{senderDirectory + "/" +
                     std::to_string(nextFreeFileNumber(senderDirectory)) + ".msg"};
    if (!of) {
        setResponse("ERR\n");
        return;
    }

    of << sender << '\n' << receiver << '\n' << subject << '\n';

    while (istringstream) {
        std::string segment;
        getline(istringstream, segment, '\n');
        of << segment << '\n';
        if (segment == ".") {
            break;
        }
    }

    setResponse("OK\n");
    of.close();
}

int CommandHandler::numberOfFiles(std::string directory) {
    int counter = 0;
    std::filesystem::path path = directory;
    for (auto &i : std::filesystem::directory_iterator(path)) {
        std::cout << i << '\n';
        counter++;
    }
    return counter;
}

int CommandHandler::nextFreeFileNumber(std::string directory) {
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

void CommandHandler::setResponse(std::string message) {
    std::string res = message;
    this->response = res.data();
    this->responseLength = res.length();
}

char *CommandHandler::getResponse() {
    return this->response;
}
int CommandHandler::getResponseLength() {
    return this->responseLength;
}
