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

const char* SERVER_IP = "127.0.0.1";   // IP e serverit

int main() {
    WSADATA wsa;
    SOCKET s = INVALID_SOCKET;
    sockaddr_in srv{};
    char buffer[BUFFER_SIZE];

    // Inicializimi i Windows Socket
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << " WSAStartup failed\n";
        return 1;
    }

	// Krijimi i socket-it UDP
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        cout << " Socket creation failed. Error: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // Konfigurimi i adreses se serverit
    srv.sin_family = AF_INET;
    int pton = inet_pton(AF_INET, SERVER_IP, &srv.sin_addr);
    if (pton != 1) {
        srv.sin_addr.s_addr = inet_addr(SERVER_IP);
    }
    srv.sin_port = htons(PORT);

    cout << " Admin client (UDP)\n";
    cout << " Server: " << SERVER_IP << ":" << PORT << "\n";
    cout << " Fillimisht jep /auth " << ADMIN_SECRET << " per privilegje.\n";
    cout << " Perdor komanden /uploadfile <path> per te ngarkuar nje file lokal.\n";

    while (true) {
        cout << "\n> ";
        string line;
        if (!getline(cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;

        //  Kontrollon nese komanda eshte /uploadfile <path>
        const string uploadFilePrefix = "/uploadfile ";
        if (line.rfind(uploadFilePrefix, 0) == 0) { 
            string localPath = line.substr(uploadFilePrefix.size());

            while (!localPath.empty() && isspace((unsigned char)localPath.front())) localPath.erase(0, 1);
            while (!localPath.empty() && isspace((unsigned char)localPath.back())) localPath.pop_back();

            // Lexo permbajtjen e file-it lokal
            ifstream ifs(localPath, ios::binary);
            if (!ifs) {
                cout << " Gabim: nuk u gjet file-i lokal: " << localPath << "\n";
                continue;
            }

            string content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
            ifs.close();

            // Nxjerr vetem emrin e file-it (pa path)
            size_t pos1 = localPath.find_last_of('/');
            size_t pos2 = localPath.find_last_of('\\');
            size_t pos = string::npos;
            if (pos1 != string::npos && pos2 != string::npos) pos = max(pos1, pos2);
            else if (pos1 != string::npos) pos = pos1;
            else if (pos2 != string::npos) pos = pos2;

            string filename = (pos == string::npos) ? localPath : localPath.substr(pos + 1);

            // Krijon komanden per serverin ne format te caktuar
            string uploadCmd = "/upload " + filename + "|" + content;

            if (uploadCmd.size() > BUFFER_SIZE) {
                cout << " File-i eshte shume i madh (" << uploadCmd.size()
                    << " byte). Rrit BUFFER_SIZE ose ndaje ne pjese me te vogla.\n";
                continue;
            }

            line = uploadCmd; 
        }

        // Dergon komanden te serveri
        sendto(
            s,
            line.c_str(),
            static_cast<int>(line.size()),
            0,
            reinterpret_cast<sockaddr*>(&srv),
            sizeof(srv)
        );

        // Merr pergjigjen nga serveri
        sockaddr_in sender{};
        int senderLen = sizeof(sender);
        memset(buffer, 0, BUFFER_SIZE);

        int got = recvfrom(
            s,
            buffer,
            BUFFER_SIZE,
            0,
            reinterpret_cast<sockaddr*>(&sender),
            &senderLen
        );

        if (got > 0) {
            cout << string(buffer, buffer + got) << "\n";
        }
        else {
            cout << "( Nuk erdhi pergjigje nga serveri)\n";
        }
    }

	// Mbyllja e socket-it
    closesocket(s);
    WSACleanup();
    cout << " Klienti u mbyll me sukses.\n";
    return 0;
}