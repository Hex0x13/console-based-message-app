#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
#define PORT 8080
#define LOCALHOST "127.0.0.1"
#define MAXCLIENT 5

typedef struct {
    char name[64];
    SOCKET sock;
    struct sockaddr_in addr;
} bindedsocket;

bindedsocket clients[MAXCLIENT] = {(bindedsocket){.sock=SOCKET_ERROR}};
int top = 0;

void throw(const char *fmt, const int N, ...);
void *recvmsg(void *arg);

int main() {
    system("cls");
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        fprintf(stderr, "WSA Startup Error\n");
        exit(EXIT_FAILURE);
    }

    SOCKET serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (serversock == -1) {
        throw("SERVER SOCKET ERROR!", 0);
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(LOCALHOST);

    if (bind(serversock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        throw("BINDING ERROR!", 1, serversock);
    }

    if (listen(serversock, MAXCLIENT) == -1) {
        throw("LISTEN ERROR!", 1, serversock);
    }

    printf("Waiting for client...\n");
    struct sockaddr_in clientaddr;
    socklen_t clientsocklen = sizeof(clientaddr);
    SOCKET clientsock;
    int arg;
    while (1)
    {
        clientsocklen = sizeof(clientaddr);
        clientsock = accept(serversock, (struct sockaddr *)&clientaddr, &clientsocklen);
        if (clientsock == -1) {
            throw("CLIENT SOCKET ERROR!", 1, serversock);
        }
        printf("CLIENT ACCEPTED(PORT: %hu, ADDR: %lu)\n", clientaddr.sin_port, clientaddr.sin_addr.s_addr);
        pthread_t th;
        arg = top;
        clients[top++] = (bindedsocket){.sock = clientsock, .addr = clientaddr};
        pthread_create(&th, NULL, recvmsg, &arg);
    }

    return 0;
}

void *recvmsg(void *arg) {
    int index = *(int *)arg;
    char recvline[1024];
    char sendline[1128];
    bindedsocket bsock = clients[index];
    recv(bsock.sock, bsock.name, sizeof(bsock.name), 0);
    while (1) {
        int recvbytes = recv(bsock.sock, recvline, sizeof(recvline) - 1, 0);
        if (recvbytes < 0) {
            throw("RECEIVING ERROR!\n", 1, bsock.sock);
        }
        printf("\rMessage from (Name: %s, PORT: %hu, ADDR: %lu): %s",
            bsock.name,
            bsock.addr.sin_port,
            bsock.addr.sin_addr.s_addr,
            recvline
        );
        sprintf(sendline, "\rfrom %s: %s", bsock.name, recvline);
        int len = strlen(sendline) + 1;
        for (int i = 0; i < top; i++) {
            if (i != index && clients[i].sock != SOCKET_ERROR) {
                send(clients[i].sock, sendline, len, 0);
            }
        }
    }
    return NULL;
}


void throw(const char *fmt, const int N, ...) {
    va_list argv;
    va_start(argv, N);
    fprintf(stderr, "%s\n", fmt);
    for (int i = 0; i < N; i++) {
        SOCKET sock = va_arg(argv, SOCKET);
        closesocket(sock);
    }
    va_end(argv);
    WSACleanup();
    exit(EXIT_FAILURE);
}