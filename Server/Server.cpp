
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include <chrono>
#include "shared.h"
#include "shared.cpp";

#pragma comment(lib, "ws2_32.lib")

using namespace std;


string makeKey(const string& ip, int port) {
    return ip + ":" + to_string(port);
}


int main() {
    WSADATA wsa;
    SOCKET sock;
    sockaddr_in srv{}, cli{};
    int cliLen = sizeof(cli);
    char buffer[BUFFER_SIZE];


    cout << "=============================\n";
    cout << "       UDP SERVER STARTED     \n";
    cout << "       Port: " << PORT << "\n";
    cout << "=============================\n\n";


    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }


    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "socket() failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(sock, (sockaddr*)&srv, sizeof(srv)) == SOCKET_ERROR) {
        cout << "bind() failed: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }



    map<string, ClientInfo> clients;
    ensureDataDir();

    while (true) {

           auto now = chrono::steady_clock::now();
        for (auto it = clients.begin(); it != clients.end(); ) {
            long long diff = chrono::duration_cast<chrono::seconds>(
                    now - it->second.lastActive
            ).count();

            if (diff > TIMEOUT_SECONDS) {
                cout << "[TIMEOUT] Klienti u fshi: " << it->first << endl;
                it = clients.erase(it);
            } else {
                ++it;
            }
        }

        memset(buffer, 0, BUFFER_SIZE);

        int got = recvfrom(sock, buffer, BUFFER_SIZE, 0, (sockaddr*)&cli, &cliLen);
        if (got == SOCKET_ERROR) {
            cerr << "recvfrom() failed: " << WSAGetLastError() << "\n";
            continue;
        } 
        
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli.sin_addr, ipStr, INET_ADDRSTRLEN);
        string ip = ipStr;

       int port = ntohs(cli.sin_port);
       string key = makeKey(ip, port);
       string msg(buffer, got);

       cout << "[COMMAND] From " << key << " -> " << msg << endl;


       auto& c = clients[key];
       c.key = key;
       c.ip = ip;
       c.port = port;
       c.messagesReceived++;
       c.totalBytes += got;
       c.lastActive = chrono::steady_clock::now();


       string reply = handleCommand(msg);

       sendto(sock, reply.c_str(), (int)reply.size(), 0, (sockaddr*)&cli, cliLen);



    }

    closesocket(sock);
    WSACleanup();
    return 0;


}