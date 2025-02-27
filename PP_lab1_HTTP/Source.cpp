#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <winsock.h>
#include <winsock2.h>
#include <sys/types.h> 
using namespace std;

char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Goodbye, world!</h1></body></html>\r\n";

void err(int code, const char buff[]) {
    cout << endl << buff;
    exit(code);
}

int main()
{
    BOOL bOptVal = TRUE, client_fd;
    int bOptLen = sizeof(BOOL);
    struct sockaddr_in svr_addr, cli_addr;
    int sin_len = sizeof(cli_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        err(1, "can't open socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen);

    int port = 8080;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) == -1) {
        closesocket(sock);
        err(1, "Can't bind");
    }

    listen(sock, 5);
    while (1) {
        client_fd = accept(sock, (struct sockaddr*)&cli_addr, &sin_len);
        printf("got connection\n");

        if (client_fd == -1) {
            perror("Can't accept");
            continue;
        }

        send(client_fd, response, sizeof(response) - 1, NULL); /*-1:'\0'*/
        closesocket(client_fd);
    }
}