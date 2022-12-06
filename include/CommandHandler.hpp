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

    std::string getMailDir();

    void parseInput(std::string input);

    char *getResponse();
    int getResponseLength();
};

#endif // COMMANDHANDLER_H_
