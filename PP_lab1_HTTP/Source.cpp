#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <sys/types.h> 
#include <pthread.h>
using namespace std;
#pragma comment (lib, "wsock32.lib")

char response1[] =
"HTTP/1.1 200 OK\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:2cm; text-align: center; color: white;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Request number ";
char response2[] = "</h1></body></html>\r\n";
static int request_number = 0;

void* thread_job(void* arg)
{
    int client_fd = (int)arg;
    //printf("got connection\n");

    if (client_fd == -1) {
        perror("Can't accept");
        return 0;
    }
    request_number += 1;
    int iResult = send(client_fd, response1, sizeof(response1)-1, NULL); /*-1:'\0'*/
    if (iResult == SOCKET_ERROR) {
        wprintf(L"send failed with error: %d, socket: %d\n", WSAGetLastError(), client_fd);
        closesocket(client_fd);
        return NULL;
    }
    //Sleep(1);
    std::string s = std::to_string(request_number);
    const char* pchar = s.c_str();
    iResult = send(client_fd, pchar, sizeof(pchar)-1, NULL); /*-1:'\0'*/
    if (iResult == SOCKET_ERROR) {
        wprintf(L"send failed with error: %d, socket: %d\n", WSAGetLastError(), client_fd);
        closesocket(client_fd);
        return NULL;
    }
    //Sleep(1);
    iResult = send(client_fd, response2, sizeof(response2)-1, NULL); /*-1:'\0'*/
    if (iResult == SOCKET_ERROR) {
        wprintf(L"send failed with error: %d, socket: %d\n", WSAGetLastError(), client_fd);
        closesocket(client_fd);
        return NULL;
    }
    s.clear();
    Sleep(1);
    shutdown(client_fd, SD_SEND);
    closesocket(client_fd);
}

void err(int code, const char buff[]) {
    cout << endl << buff;
    exit(code);
}

int main()
{
    int retval;
    BOOL bOptVal = TRUE;
    int bOptLen = sizeof(BOOL);
    struct sockaddr_in svr_addr, cli_addr;
    int sin_len = sizeof(cli_addr);
    //---------------------------------------
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"Error at WSAStartup()\n");
        return 1;
    }
    //---------------------------------------
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        err(1, "can't open socket");
    cout << "Listen socket created" << endl;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen);

    int port = 12435;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) == -1) {
        closesocket(sock);
        WSACleanup();
        err(1, "Can't bind");
    }

    listen(sock, 5);
    cout << "Listening..." << endl;
    while (1) {
        int client_fd = accept(sock, (struct sockaddr*)&cli_addr, &sin_len);
        if (client_fd == INVALID_SOCKET) {
            wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
            continue;
        }

        // Создание потока
        pthread_t threads;
        retval = pthread_create(&threads, NULL, thread_job, (int*)client_fd);
        // Если при создании потока произошла ошибка, выводим
        // сообщение об ошибке и прекращаем работу программы
        if (retval != 0) {
            cout << "Не получилось создать поток: " << strerror(retval) << endl;
            exit(-1);
        }
    }
    WSACleanup();
}