#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#define _WIN32_WINNT 0x0600
#define _WIN32_WINNT 0x0601
#define _WIN32_WINNT 0x0602
#endif //
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>

#endif
#if defined(_WIN32)
#define ISVALIDSOCKET(s)((s)!=INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s)>=0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif // defined

#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;
int main() {
    // Initialize Dependencies to the Windows Socket.
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << GETSOCKETERRNO()<<"\n";
        system("pause");
        return -1;
    }

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;        
    hints.ai_protocol = IPPROTO_TCP;  
    hints.ai_socktype = SOCK_STREAM;   

    struct addrinfo* targetAdressInfo = NULL;
    DWORD getAddrRes = getaddrinfo("app.sportdataapi.com", NULL, &hints, &targetAdressInfo);
    cout << getAddrRes<<"\n";
    cout << targetAdressInfo<<"\n";
    if (getAddrRes != 0 || targetAdressInfo == NULL)
    {
        cout << GETSOCKETERRNO() << "\n";
        system("pause");
        WSACleanup();
        return -1;
    }

    SOCKADDR_IN sockAddr;
    sockAddr.sin_addr = ((struct sockaddr_in*)targetAdressInfo->ai_addr)->sin_addr; 
    sockAddr.sin_family = AF_INET; 
    sockAddr.sin_port = htons(80); 

    freeaddrinfo(targetAdressInfo);

    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket == INVALID_SOCKET)
    {
        cout << GETSOCKETERRNO() << "\n";
        system("pause");
        WSACleanup();
        return -1;
    }

    cout << "Connecting...\n";
    if (connect(Socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) != 0)
    {
        cout << GETSOCKETERRNO() << "\n";
        system("pause");
        closesocket(Socket);
        WSACleanup();
        return -1;
    }
    cout << "Connected.\n";

    const char* httpsRequest = "GET /api/v1/soccer/seasons HTTP/1.1\r\nHost: app.sportdataapi.com\r\napikey : 8219c9a0-49a2-11ed-80a8-7739b48a761e\r\nleague_id:392\r\n\r\n";
    int sentBytes = send(Socket, httpsRequest, strlen(httpsRequest), 0);
    if (sentBytes < strlen(httpsRequest) || sentBytes == SOCKET_ERROR)
    {
        cout << GETSOCKETERRNO() << "\n";
        system("pause");
        closesocket(Socket);
        WSACleanup();
        return -1;
    }

    char buffer[10000];
    ZeroMemory(buffer, sizeof(buffer));
    int dataLen;
    while ((dataLen = recv(Socket, buffer, sizeof(buffer), 0) > 0))
    {
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            cout << buffer[i];
            i += 1;
        }
    }

    CLOSESOCKET(Socket);
    WSACleanup();

    system("pause");
    return 0;
}
