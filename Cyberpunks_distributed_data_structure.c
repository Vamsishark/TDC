#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define MAX_NODES 10
#define MAX_BUFFER_SIZE 1024

struct Node {
    char data[MAX_BUFFER_SIZE];
    struct Node* next;
};

struct Node* head = NULL;

void appendToList(char* data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->data, data);
    newNode->next = head;
    head = newNode;
}

void printList() {
    struct Node* current = head;
    printf("List contents:\n");
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    // Initialize the list
    head = NULL;

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error in binding");
        exit(1);
    }

    // Listen for connections
    listen(serverSocket, MAX_NODES);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connections
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);

        // Receive data from the client
        char data[MAX_BUFFER_SIZE];
        recv(clientSocket, data, MAX_BUFFER_SIZE, 0);
        printf("Received data: %s\n", data);

        // Append received data to the distributed list
        appendToList(data);

        // Print the updated list
        printList();

        // Close the client socket
        close(clientSocket);
    }

    return 0;
}
