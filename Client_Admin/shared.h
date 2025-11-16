#pragma once
#include <string>
#include <chrono>
using namespace std;

extern const string ADMIN_SECRET;
extern const char* DATA_DIR;

extern const int PORT;
extern const int TIMEOUT_SECONDS;

#define BUFFER_SIZE 1024

extern bool isAuthenticated; 

struct ClientInfo {
    string key, ip;
    int port = 0;
    bool isAdmin = false;
    int messagesReceived = 0;
    int totalBytes = 0;
    chrono::steady_clock::time_point lastActive;
};

void ensureDataDir();
std::string listFiles();
std::string readFile(const std::string& filename);
std::string deleteFile(const std::string& filename);
std::string infoFile(const std::string& filename);
std::string searchFiles(const std::string& keyword);
std::string uploadFile(const std::string& filename, const std::string& content);
std::string downloadFile(const std::string& filename);
std::string handleCommand(const std::string& input);
