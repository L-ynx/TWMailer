#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_
#include <mutex>
#include <string>
#include <unordered_map>

class CommandHandler {
  private:
    static const int DELAY = 60;
    static const int ATTEMPT_LIMIT = 3;

    std::string maildir;
    std::string response;
    int responseLength;

    int failedAttempts = 0;

    std::unordered_map<std::string, int> blacklist;

    std::string command;
    std::string sender;
    std::string senderIP;
    std::string messageNumber;

    // https://stackoverflow.com/a/5429695
    std::mutex _mutex;

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
    std::string trimPW(std::string input);

    int numberOfFiles(std::string directory);
    int nextFreeFileNumber(std::string directory);

    void setSenderIP(std::string ip);
    void setResponse(std::string message);
    std::string getResponse();
    int getResponseLength();

    void blacklistSender();
    bool isBlacklisted();
    void readBlacklist();
    void saveBlacklist();
};

#endif // COMMANDHANDLER_H_
