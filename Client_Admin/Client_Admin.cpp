#include <iostream>
#include <string>
#include "shared.h"
#include <winsock2.h>

using namespace std;

const char* SERVER_IP = "127.0.0.1";

int main()
{
    WSADATA wsa;
    SOCKET s = INVALID_SOCKET;
    char buffer[BUFFER_SIZE];

	//Inicializimi i Windows Socket
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << " WSAStartup deshtoi\n";
        return 1;
    }

	//Krijimi i socketit UDP
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        cout << "Krijimi i socket-it deshtoi. Error: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    //Mbyllja e socketit
    closesocket(s);
    WSACleanup();
    cout << " Klienti u mbyll me sukses.\n";
    return 0;
}