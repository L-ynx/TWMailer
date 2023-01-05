#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_
#include <string>

class CommandHandler {
  private:
    std::string maildir;
    std::string response;
    int responseLength;

    std::string command;
    std::string sender;
    std::string messageNumber;

  public:
    CommandHandler(std::string directory);
    void createDirectory(std::string directory);

    std::string getMailDir();

    void parseInput(std::string input);
    void saveMessage(std::string input);
    void listMessages(std::string input);
    void readMessage(std::string input);
    void deleteMessage(std::string input);
    void attemptLogin(std::string input);

    std::string readCommand(std::string input);
    std::string readSender(std::string input);
    std::string readMessageNumber(std::string input);
    std::string trim(std::string input);

    int numberOfFiles(std::string directory);
    int nextFreeFileNumber(std::string directory);

    void setResponse(std::string message);
    std::string getResponse();
    int getResponseLength();
};

#endif // COMMANDHANDLER_H_
