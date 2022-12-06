#include "CommandHandler.hpp"

CommandHandler::CommandHandler(std::string directory) {
    this->maildir = directory;
}

void CommandHandler::parseInput(std::string input) {
    this->response = input.data();
    this->responseLength = input.length();
}

char *CommandHandler::getResponse() {
    return this->response;
}
int CommandHandler::getResponseLength() {
    return this->responseLength;
}
