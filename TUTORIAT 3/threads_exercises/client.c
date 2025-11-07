#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#define PORT 8081 
#define THREAD_COUNT 3

void printAddress(const struct sockaddr_in* addr){
    char ip[INET_ADDRSTRLEN];  // for IPv4
    int port;

    inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
    port = ntohs(addr->sin_port);

    printf("(%s, %d)", ip, port);
}


void* create_new_conn(void* args){
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Print connection information
    printf("Thread %ld: Established connection to ", pthread_self());
    printAddress(&serv_addr);
    printf("\n");

    // Set-up message
    char msg[50] = {0};
    sprintf(msg, "Hello from client thread %ld", pthread_self());
    
    // Send meesage
    send(sock, msg, strlen(msg), 0);
    printf("Thread %ld: Message sent: %s\n", pthread_self(), msg);

    // Receive response
    char buffer[1024] = {0};
    recv(sock, buffer, sizeof(buffer), NULL);
    printf("Thread %ld: Received from server: %s\n", pthread_self() , buffer);

    // Clean-up
    close(sock);

    return NULL;
}

// Creates multiple connections to the server using multiple threads
int main() {
   pthread_t threads[THREAD_COUNT];
   // Each thread will create and handle a connection
   for (int i = 0; i < THREAD_COUNT; ++i){
        errno = pthread_create(&threads[i], NULL, create_new_conn, NULL);
        if (errno){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
   }
   create_new_conn(NULL);

   // Join threads
   for (int i = 0; i < THREAD_COUNT; ++i){
        errno = pthread_join(threads[i], NULL);
        if (errno){
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
   }
   return 0;
}