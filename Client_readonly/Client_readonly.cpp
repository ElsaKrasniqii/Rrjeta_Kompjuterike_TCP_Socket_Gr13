#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "shared.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define SERVER_IP "127.0.0.1"  // ketu vendoset IP-ja e cila do te perdoret ne momentin kur laptopi lidhet me rrjet hotspot

int main() {
    WSADATA wsa;
    SOCKET s;
    sockaddr_in srv{};
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "WSAStartup failed.\n";
        return 1;
    }

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) {
        cout << "socket() failed.\n";
        WSACleanup();
        return 1;
    }


    srv.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &srv.sin_addr);
    srv.sin_port = htons(PORT);

    cout << "Read-only client (UDP). Server: " << SERVER_IP << ":" << PORT << "\n";
    cout << "Komanda te lejuara: /read <file>, /search <word>, /list\n";


}