#include <winsock.h>
#include <stdio.h>

static const int maxNumClients = 3;
static const int defaultMessageLen = 1024;
static const int usernameLen = 10;

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    printf("START OF LOG\n");
    SOCKET server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    bind(server_socket, (struct sockaddr*)&server, sizeof(server));
    listen(server_socket, 3);  // backlog parameter is setting the limit on the connection queue doesn't really matter

    struct sockaddr_in clientObject;
    int clientObjectLen = sizeof(clientObject);
    int numClients = 0;
    SOCKET clientSockets[maxNumClients];
    char usernameArr[maxNumClients][usernameLen];

    int firstClientConnected = 0;

    while (1) {
        // Initialize the file descriptor set for the available clients
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        for (int i = 0; i < numClients; i++) {
            FD_SET(clientSockets[i], &readfds);
        }
        // Call select to mutate the fd set for only active sockets
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int activity = select(maxNumClients + 1, &readfds, NULL, NULL, &timeout);
        if (activity == SOCKET_ERROR) {
            printf("%s", GetLastError());
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }

        // Now we iterate over the clientSockets to see if their respective fds are in the
        // fd set, and to then accept the connection and add them to 
        if (FD_ISSET(server_socket, &readfds)) {
            SOCKET firstClientSocket = accept(server_socket, (struct sockaddr*)&clientObject, &clientObjectLen);
            if (firstClientSocket != -1 && numClients < maxNumClients) {
                clientSockets[numClients] = firstClientSocket;
                char username[usernameLen];
                int formattedSize = usernameLen + 17;
                char formattedConnectionMsg[formattedSize];
                int receivedMessage = recv(firstClientSocket, username, sizeof(username) - 1, 0);
                if (receivedMessage > 0) {
                    // Add the username to the array
                    strcpy(usernameArr[numClients], username);
                    snprintf(formattedConnectionMsg, formattedSize, "<%s has connected>", username);
                    printf("%s\n", formattedConnectionMsg);

                    for (int i = 0; i < numClients; i++) {
                        send(clientSockets[i], formattedConnectionMsg, strlen(formattedConnectionMsg), 0);
                    }
                    firstClientConnected = 1;
                    numClients++;
                }
            }            
        }

        // Check available and ready sockets (in fd_set) for messages
        for (int i = 0; i < numClients; i++) {
            if (FD_ISSET(clientSockets[i], &readfds)) {
                char messageBuffer[defaultMessageLen];

                int availableMessage = recv(clientSockets[i], messageBuffer, sizeof(messageBuffer) - 1, 0);
                if (availableMessage > 0) {
                    // If the message is valid
                    messageBuffer[availableMessage] = '\0';
                    int formattedSize = defaultMessageLen + sizeof(usernameArr[i]);
                    char formattedBuffer[formattedSize];  // The message formatted with the username of the client
                    snprintf(formattedBuffer, formattedSize, "%s: %s", usernameArr[i], messageBuffer);
                    printf("%s\n", formattedBuffer);
                    // Relay the received message to every client (including the client the message was from)
                    for (int j = 0; j < numClients; j++) {
                        if (clientSockets[j] != clientSockets[i]) {
                            printf("sent to %s\n", usernameArr[j]);
                            send(clientSockets[j], formattedBuffer, strlen(formattedBuffer), 0);
                        }
                    }
                }
                else {
                    closesocket(clientSockets[i]);
                    printf("%s has Disconnected\n", usernameArr[i]);
                    for (int j = i; j < numClients - 1; j++) {
                        clientSockets[j] = clientSockets[j + 1];
                    }
                    numClients--;
                    if (numClients == 0 && firstClientConnected == 1) {
                        break;
                    }
                }
            }
        }
    }
    closesocket(server_socket);
    WSACleanup();
    return 0;
}