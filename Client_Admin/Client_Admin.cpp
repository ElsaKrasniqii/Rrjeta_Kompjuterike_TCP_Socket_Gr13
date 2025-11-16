#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "shared.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const char* SERVER_IP = "127.0.0.1";

int main() {

    WSADATA wsa;
    SOCKET s = INVALID_SOCKET;
    sockaddr_in srv{};
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    // Create UDP socket
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        return 1;
    }

    // Configure server address
    srv.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &srv.sin_addr);
    srv.sin_port = htons(PORT);

    cout << "Admin UDP client.\n";
    cout << "Use /auth " << ADMIN_SECRET << "\n";
    cout << "Use /uploadfile <path>\n";
    cout << "Use /download <filename>\n";

    while (true) {
        cout << "\n> ";
        string line;
        getline(cin, line);

        if (line == "exit") break;
        if (line.empty()) continue;

        // Upload file
        const string uploadPrefix = "/uploadfile ";
        if (line.rfind(uploadPrefix, 0) == 0) {

            string localPath = line.substr(uploadPrefix.size());

            // Trim spaces
            while (!localPath.empty() && isspace((unsigned char)localPath.front()))
                localPath.erase(0, 1);

            while (!localPath.empty() && isspace((unsigned char)localPath.back()))
                localPath.pop_back();

            // Open local file
            ifstream ifs(localPath, ios::binary);
            if (!ifs) {
                cout << "File not found: " << localPath << "\n";
                continue;
            }

            // Read file content
            string content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

            // Extract filename
            size_t pos = localPath.find_last_of("/\\");
            string filename = (pos == string::npos) ? localPath : localPath.substr(pos + 1);

            // Build upload command
            string uploadCmd = "/upload " + filename + "|" + content;

            if (uploadCmd.size() > BUFFER_SIZE) {
                cout << "File too large.\n";
                continue;
            }

            line = uploadCmd;
        }

        // Download file
        const string dlPrefix = "/download ";
        bool isDownload = false;
        string dlFilename;

        if (line.rfind(dlPrefix, 0) == 0) {
            dlFilename = line.substr(dlPrefix.size());
            isDownload = true;
        }

        // Send command to server
        sendto(s, line.c_str(), static_cast<int>(line.size()), 0, (sockaddr*)&srv, sizeof(srv));

        // Receive response
        sockaddr_in sender{};
        int sl = sizeof(sender);
        memset(buffer, 0, BUFFER_SIZE);

        int got = recvfrom(s, buffer, BUFFER_SIZE, 0, (sockaddr*)&sender, &sl);
        if (got <= 0) {
            cout << "(No response)\n";
            continue;
        }

        string response(buffer, got);

        // If this is a download, save file locally
        if (isDownload) {

            if (response.rfind("ERROR", 0) == 0) {
                cout << response << "\n";
                continue;
            }

            // Save as downloaded_<filename>
            string outPath = "downloaded_" + dlFilename;
            ofstream ofs(outPath, ios::binary);

            if (!ofs) {
                cout << "Failed to save file locally.\n";
            }
            else {
                ofs << response;
                ofs.close();
                cout << "File downloaded and saved as: " << outPath << "\n";
            }

            continue;
        }

        // Normal server response
        cout << response << "\n";
    }

    // Close socket
    closesocket(s);
    WSACleanup();
    return 0;
}
