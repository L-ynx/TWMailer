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
    getline(istringstream, segment);

    if (segment == "SEND") {
        saveMessage(input.erase(0, 5));
    } else if (segment == "LIST") {
        listMessages(input.erase(0, 5));
    } else if (segment == "READ") {
        readMessage(input.erase(0, 5));
    } else if (segment == "DEL") {
        deleteMessage(input.erase(0, 4));
    }
}

void CommandHandler::saveMessage(std::string input) {
    std::stringstream istringstream(input);
    std::string sender;
    getline(istringstream, sender);

    std::string senderDirectory = this->maildir + "/" + sender;
    createDirectory(senderDirectory);

    std::ofstream of{senderDirectory + "/" +
                     std::to_string(nextFreeFileNumber(senderDirectory)) + ".msg"};
    if (!of) {
        setResponse("ERR\n");
        return;
    }

    of << sender << '\n';

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
    std::string sender;
    getline(istringstream, sender);

    std::string senderDirectory = this->maildir + "/" + sender;
    std::string response;

    if (!std::filesystem::exists(senderDirectory)) {
        response = "0";
    } else if (numberOfFiles(senderDirectory) == 0) {
        response = "0";
    } else {
        response += std::to_string(numberOfFiles(senderDirectory)) + "\n";
        std::filesystem::path path = senderDirectory;
        for (auto &file : std::filesystem::directory_iterator(path)) {
            std::ifstream is{file.path()};
            std::string sender, receiver, subject;
            getline(is, sender);
            getline(is, receiver);
            getline(is, subject);
            response += subject + "\n";
            is.close();
        }
    }
    setResponse(response);
}

void CommandHandler::readMessage(std::string input) {
    std::stringstream istringstream(input);
    std::string sender;
    getline(istringstream, sender);
    std::string messageNumber;
    getline(istringstream, messageNumber);

    std::string senderDirectory = this->maildir + "/" + sender;
    std::string filename = senderDirectory + "/" + messageNumber + ".msg";
    std::string response;

    if (!std::filesystem::exists(senderDirectory)) {
        response = "ERR\n";
    } else if (numberOfFiles(senderDirectory) == 0) {
        response = "ERR\n";
    } else if (!std::filesystem::exists(
                   std::filesystem::directory_entry(filename))) {
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

int CommandHandler::numberOfFiles(std::string directory) {
    int counter = 0;
    std::filesystem::path path = directory;
    for (auto &file : std::filesystem::directory_iterator(path)) {
        std::cout << file.path() << '\n';
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
    this->response = message.data();
    this->responseLength = message.length();
}

char *CommandHandler::getResponse() {
    return this->response;
}
int CommandHandler::getResponseLength() {
    return this->responseLength;
}
