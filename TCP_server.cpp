#ifdef WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <thread>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>

void InitializeSockets()
{
#ifdef WIN32
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0)
    {
        printf("WSAStartup failed with error: %d\n", res);
        return;
    }
#endif // WIN32
}

void UninitializeSockets()
{
#ifdef WIN32
    WSACleanup();
#endif
}

void ClientHandler(int client_sock)
{
    while (true)
    {
        char buff[11]="";
        int size = recv(client_sock, buff, 11, 0);
        if (size<=0)
        {
            printf("Client %d exit\n", client_sock);
            closesocket(client_sock);
            return;
        }

        printf("Received message: %s\n", buff);
        int number = std::atoi(buff);

        std::string answer;

        if (number == 0)
        {
            answer = "Wrong msg";
        }
        else
        {
            number += number;
            answer = std::to_string(number);
        }

        size = send(client_sock, answer.c_str(), answer.size(), 0);
        if (size != answer.size())
        {
            if (size <= 0)
            {
                printf("Network error\n");
            }
            else
            {
                printf("Network buffer error\n");
            }
            return;
        }
    }
}

int main()
{
    InitializeSockets();

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        printf("Error of sockets created\n");
        return -1;
    }

    struct sockaddr_in myaddress;
    myaddress.sin_family = AF_INET;
    myaddress.sin_port = htons(50011);
    myaddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr*)&myaddress, sizeof(myaddress)) < 0)
    {
        printf("Error of binding\n");
        closesocket(sock);
        UninitializeSockets();
        return -1;
    }

    int code = listen(sock, 25);
    if (code < 0)
    {
        printf("Listen error\n");
        closesocket(sock);
        UninitializeSockets();
        return -1;
    }

    while (true)
    {
        int client_sock = accept(sock, NULL, NULL);
        if (client_sock <= 0)
        {
            printf("Sockets making error\n");
            closesocket(sock);
            UninitializeSockets();
            return -1;
        }

        std::thread client_thread = std::thread(ClientHandler, client_sock);
        client_thread.detach();

    }

}
