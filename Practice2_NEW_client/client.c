#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#define MAX_SIZE 1000
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 6379
#define BUFFER_SIZE 104857600

int main() {
    WSADATA wsaData;
    SOCKET ClientSock;
    SOCKADDR_IN ServerInfo;
    int exodus;

    // Инициализируем библиотеку Winsock
    exodus = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (exodus != 0) {
        printf("WSAStartup failed: %d\n", exodus);
        return 1;
    }

    // Создаем сокет для клиента
    ClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ClientSock == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Подключаемся к серверу
    ZeroMemory(&ServerInfo, sizeof(ServerInfo));
    ServerInfo.sin_family = AF_INET;
    ServerInfo.sin_addr.s_addr = inet_addr("127.0.0.1"); // адрес сервера
    ServerInfo.sin_port = htons(PORT); // порт сервера
    exodus = connect(ClientSock, (SOCKADDR*)&ServerInfo, sizeof(ServerInfo));
    if (exodus == SOCKET_ERROR) {
        printf("connect failed: %d\n", WSAGetLastError());
        closesocket(ClientSock);
        WSACleanup();
        return 1;
    }

    // Отправляем и получаем данные от сервера
    printf("Connected to server on port %d\n", PORT);
    char* buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
    int bytes_sent, bytes_received;
    while (1) {
        // Вводим запрос
        printf("Enter your query: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // удаляем символ перевода строки

        // Отправляем запрос
        bytes_sent = send(ClientSock, buffer, strlen(buffer), 0);
        if (bytes_sent == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            break;
        }

        // Получаем ответ
        bytes_received = recv(ClientSock, buffer, BUFFER_SIZE, 0);
        if (bytes_received == SOCKET_ERROR) {
            printf("recv failed: %d\n", WSAGetLastError());
            break;
        }
        buffer[bytes_received] = '\0';

        // Выводим ответ
        printf("Server response: %s\n", buffer);
    }

    // Закрываем сокет клиента и очищаем библиотеку Winsock
    free(buffer);
    closesocket(ClientSock);
    WSACleanup();
    return 0;
}