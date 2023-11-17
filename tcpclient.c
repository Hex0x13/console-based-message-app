#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <ws2tcpip.h>
#include <pthread.h>
#define PORT 8080
#define LOCALHOST "127.0.0.1"
#define MAXLINE 1024

void throw(const char *fmt, const int N, ...);
void *recvmsg(void *arg);

int main() {
    system("cls");
    char name[64];
    printf("Enter username: ");
    scanf(" %63[^\n]", name);
    while(getchar() != '\n');

    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        fprintf(stderr, "WSADATA Startup Error!\n");
        exit(EXIT_FAILURE);
    }

    SOCKET clientsock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsock == -1) {
        throw("SOCKET ERROR!", 0);
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(LOCALHOST);

    if (connect(clientsock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        throw("CONNECTING FAILED!", 1, clientsock);
    }
    printf("CONNECTED TO %lu\n", serveraddr.sin_addr.s_addr);
    send(clientsock, name, strlen(name) + 1, 0);
    
    char sendline[MAXLINE];
    pthread_t th;
    pthread_create(&th, NULL, recvmsg, &clientsock);
    while (1) {
        printf("\rEnter Message: ");
        fgets(sendline, MAXLINE, stdin);
        send(clientsock, sendline, strlen(sendline) + 1, 0);
    }

    return EXIT_SUCCESS;
}

void *recvmsg(void *arg) {
    char recvline[1128];
    SOCKET sock = *(SOCKET *)arg;
    while (1) {
        int recvbytes = recv(sock, recvline, sizeof(recvline), 0);
        if (recvbytes < 0) {
            throw("RECEIVING ERROR!\n", 1, sock);
        }
        printf("%s", recvline);
        printf("\rEnter Message: ");
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