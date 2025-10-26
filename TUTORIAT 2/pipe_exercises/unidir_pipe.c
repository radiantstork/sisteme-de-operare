#include <unistd.h>
#include <stdio.h>
#include <string.h>

// Not all the neccessary error checking was done (aka for read and write)
int main(){
    int fd[2];
    char nr;
    if (pipe(fd) == -1){
        perror("pipe:");
        return -1;
    }

    int child_pid = fork();
    if (child_pid == -1){
        perror("fork:");
        return -1;
    }
    // Child section
    else if (child_pid == 0) {
        close(fd[1]); // Close write fd
        while (read(fd[0], &nr, 1) > 0)
            printf("Received byte %d from parent\n", nr);
        
        close(fd[0]); // Close read fd
        exit(0);
    }
    // Parent executes
    else{
        close(fd[0]); // Close read fd
        for (char c = 0; c < 10; ++c){
            printf("Sending byte %d to child\n", c);
            write(fd[1], &c, 1);
        }
        close(fd[1]); // Close write fd
        wait(NULL);
        exit(0);
    }
}