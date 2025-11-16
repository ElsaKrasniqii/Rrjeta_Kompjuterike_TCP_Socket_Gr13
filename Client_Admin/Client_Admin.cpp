#define _HAS_STD_BYTE 0
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "shared.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <filesystem>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
namespace fs = std::filesystem;

const char* SERVER_IP = "172.20.10.3";

int main() {

    WSADATA wsa;
    SOCKET s = INVALID_SOCKET;
    sockaddr_in srv{};
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        return 1;
    }

    srv.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &srv.sin_addr);
    srv.sin_port = htons(PORT);

    cout << "Admin UDP client.\n";
    cout << "Use /auth " << ADMIN_SECRET << "\n";
    cout << "Use /upload <path>\n";
    cout << "Use /info  <filename>\n";
    cout << "Use /download <filename>\n";
    cout << "Use /delete <filename>\n";

    string downloadFolder = "Downloads";

    if (!fs::exists(downloadFolder)) {
        fs::create_directory(downloadFolder);
    }

    while (true) {
        cout << "\n> ";
        string line;
        getline(cin, line);

        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;

        const string uploadPrefix = "/uploadfile ";
        if (line.rfind(uploadPrefix, 0) == 0) {

            string localPath = line.substr(uploadPrefix.size());

            while (!localPath.empty() && isspace((unsigned char)localPath.front()))
                localPath.erase(0, 1);

            while (!localPath.empty() && isspace((unsigned char)localPath.back()))
                localPath.pop_back();


            ifstream ifs(localPath, ios::binary);
            if (!ifs) {
                cout << "File not found: " << localPath << "\n";
                continue;
            }

            string content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
            size_t pos = localPath.find_last_of("/\\");
            string filename = (pos == string::npos) ? localPath : localPath.substr(pos + 1);

            string uploadCmd = "/upload " + filename + "|" + content;

            if (uploadCmd.size() > BUFFER_SIZE) {
                cout << "File too large.\n";
                continue;
            }

            line = uploadCmd;
        }

        const string dlPrefix = "/download ";
        bool isDownload = false;
        string dlFilename;

        if (line.rfind(dlPrefix, 0) == 0) {
            dlFilename = line.substr(dlPrefix.size());
            isDownload = true;
        }

        sendto(s, line.c_str(), (int)line.size(), 0, (sockaddr*)&srv, sizeof(srv));

        sockaddr_in sender{};
        int sl = sizeof(sender);
        memset(buffer, 0, BUFFER_SIZE);

        int got = recvfrom(s, buffer, BUFFER_SIZE, 0, (sockaddr*)&sender, &sl);
        if (got <= 0) {
            cout << "(No response)\n";
            continue;
        }

        string response(buffer, got);

        if (isDownload) {

            if (response.rfind("ERROR", 0) == 0) {
                cout << response << "\n";
                continue;
            }

            string outPath = downloadFolder + "/downloaded_" + dlFilename;

            ofstream ofs(outPath, ios::binary);

            if (!ofs) {
                cout << "Failed to save file at: " << outPath << "\n";
            }
            else {
                ofs << response;
                ofs.close();
                cout << "File downloaded and saved to:\n" << outPath << "\n";
            }

            continue;
        }

        cout << response << "\n";
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
