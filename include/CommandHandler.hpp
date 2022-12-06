#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_
#include <string>

class CommandHandler {
  private:
    std::string maildir;

  public:
    CommandHandler(std::string directory);

    std::string getMailDir();

    void parseInput(std::string input);
};

#endif // COMMANDHANDLER_H_
