#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#define PORT 8081 // Should match the one in client
#define EXPECTED_REQUESTS 4 //Should match THREAD_COUNT + 1 from client


void printAddress(const struct sockaddr_in* addr){
    char ip[INET_ADDRSTRLEN];  // for IPv4
    int port;

    inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
    port = ntohs(addr->sin_port);

    printf("(%s, %d)", ip, port);
}

// Received message from client and sends a response
// args: client_fd, responsible with calling close(client_fd) at the end
// ret: NULL
void* handle_conn(void* args){
    int client_fd = *(int*)args;

    // Receive data
    char buffer[1024] = {0};
    recv(client_fd, buffer, sizeof(buffer), NULL);
    printf("Thread %ld: Received from client: %s\n", pthread_self(), buffer);

    // Set-up response
    char msg[50] = {0};
    sprintf(msg, "Hello from server thread %ld", pthread_self());

    // Send response
    send(client_fd, msg, strlen(msg), 0);
    printf("Thread %ld: Hello message sent: %s\n", pthread_self(), msg);

    // Clean-up
    close(client_fd);

    return NULL;
}

// Creates a socket that listens for incoming tcp requests that will be handled by a separate thread
int main() {
    int server_fd;
    struct sockaddr_in address;

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set address info
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept any interface, including loopback
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    pthread_t threads[EXPECTED_REQUESTS];
    int client_fds[EXPECTED_REQUESTS];

    // For each request create a new thread
    for (int i = 0; i < EXPECTED_REQUESTS; ++i){
        // Accept a connection
        socklen_t addrlen = sizeof(address);
        client_fds[i] = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fds[i] < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        
        // Print connection information
        printf("Thread %ld: Accepted new connection from ", pthread_self());
        printAddress(&address);
        printf("\n");

        // Create thread for new accepted connection
        errno = pthread_create(&threads[i], NULL, handle_conn, &client_fds[i]);
        if (errno){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Join with threads
    for (int i = 0; i < EXPECTED_REQUESTS; ++i){
        errno = pthread_join(threads[i], NULL);
        if (errno){
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    // Clean-up
    close(server_fd);
    return 0;
}