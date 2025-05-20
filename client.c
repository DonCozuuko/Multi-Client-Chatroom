#include <winsock2.h>
#include <stdio.h>
#include <unistd.h>
#include <conio.h>

static const int defaultMessageLen = 1024;
static const int usernameLen = 12;

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8080);

    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) == -1) {
        // If there is not a successfull connection
        printf("%s", GetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    
    printf("Connected to Server\n");

    char username[usernameLen];
    while (1) {
        printf("Enter Username: ");
        fgets(username, sizeof(username), stdin);

        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n') {
            username[len - 1] = '\0';
            len--;
        }
        if (len >= 10) {
            printf("Username too long\n");
            continue;
        }
        else {
            send(client_socket, username, strlen(username), 0);
            printf("\n");
            break;
        }
    }
    int noRelayedMsgs = 0;
    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(client_socket, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;  // 0ms select timeout

        int activity = select(0, &readfds, NULL, NULL, &timeout);
        if (activity == SOCKET_ERROR) {
            printf("Select failed: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(client_socket, &readfds)) {
            char recvBuffer[defaultMessageLen + usernameLen + 1];
            int bytesReceived = recv(client_socket, recvBuffer, sizeof(recvBuffer) - 1, 0);
            if (bytesReceived > 0) {
                recvBuffer[bytesReceived] = '\0';
                printf("\r\033[K");
                fflush(stdout);
                printf("%s\n", recvBuffer);
                noRelayedMsgs = 0;
            }
        }

        // Check for keyboard input (user wants to send a message)
        char message[defaultMessageLen];
        if (noRelayedMsgs == 0) {
            printf("Message: ");
            noRelayedMsgs = 1;
        }
        if (_kbhit()) {
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = '\0';  // remove newline
            printf("\033[F");  // moves cursor up one line
            printf("\033[K");  // deletes line with cursor
            printf("%s: %s\n", username, message);
            send(client_socket, message, strlen(message), 0);
            noRelayedMsgs = 0;
        }
    }
    closesocket(client_socket);
    WSACleanup();
    return 0;
}