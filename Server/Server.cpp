
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


    cout << "UDP server u nis ne portin: " << PORT << "\n";




}