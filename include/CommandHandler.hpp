#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_
#include <string>

class CommandHandler {
  private:
    std::string maildir;
    char *response;
    int responseLength;

  public:
    CommandHandler(std::string directory);
    void createDirectory(std::string directory);

    std::string getMailDir();

    void parseInput(std::string input);
    void saveMessage(std::string message);

    int numberOfFiles(std::string directory);
    int nextFreeFileNumber(std::string directory);

    void setResponse(std::string message);
    char *getResponse();
    int getResponseLength();
};

#endif // COMMANDHANDLER_H_
