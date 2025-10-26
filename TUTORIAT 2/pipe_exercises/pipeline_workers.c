#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

// Define pipe names - must be consistent across processes
const char *pipe_boss_to_worker_parent_name = "/tmp/pipe_boss_to_worker_parent";
const char *pipe_worker_child_to_boss_name = "/tmp/pipe_worker_child_to_boss";

void child_worker_run(int read_fd_from_parent) {
    int pipe_worker_child_to_boss = -1;
    char byte_in, byte_out;

    // Open named pipe to BOSS for writing
    // This will block until BOSS opens it for reading
    pipe_worker_child_to_boss = open(pipe_worker_child_to_boss_name, O_WRONLY);
    if (pipe_worker_child_to_boss == -1) {
        perror("  CHILD WORKER open pipe_worker_child_to_boss_name for writing");
        exit(1);
    }
    printf("  CHILD WORKER: Pipe to BOSS connected.\n");

    // Read from PARENT WORKER (ordinary pipe), process, write to BOSS (named pipe)
    while (read(read_fd_from_parent, &byte_in, sizeof(byte_in)) > 0) {
        printf("  CHILD WORKER: Received byte %d from PARENT WORKER\n", byte_in);
        
        byte_out = byte_in + 10;

        printf("  CHILD WORKER: Sending byte %d to BOSS\n", byte_out);
        if (write(pipe_worker_child_to_boss, &byte_out, sizeof(byte_out)) == -1) {
            perror("  CHILD WORKER write");
            close(read_fd_from_parent);
            close(pipe_worker_child_to_boss);
            exit(1);
        }
    }

    // Clean up
    printf("  CHILD WORKER: Pipe from PARENT WORKER closed. Exiting.\n");
    close(read_fd_from_parent); // Close read end of ordinary pipe
    close(pipe_worker_child_to_boss); // Close write end to BOSS (sends EOF)
    exit(0);
}


int main() {
    int pipe_boss_to_worker_parent = -1; // Named pipe fd between boss and worker parnet
    int pipe_workers[2]; // Ordinary pipe: [0]=read, [1]=write
    pid_t child_pid;

    // Open named pipe from BOSS for reading
    // This will block until BOSS opens it for writing
    printf("PARENT WORKER: Waiting to open pipe from BOSS...\n");
    pipe_boss_to_worker_parent = open(pipe_boss_to_worker_parent_name, O_RDONLY);
    if (pipe_boss_to_worker_parent == -1) {
        perror("PARENT WORKER open pipe_boss_to_worker_parent for reading");
        exit(1);
    }
    printf("PARENT WORKER: Pipe from BOSS connected.\n");

    // Create the ordinary pipe for PARENT WORKER -> CHILD WORKER
    if (pipe(pipe_workers) == -1) {
        perror("PARENT WORKER pipe");
        close(pipe_boss_to_worker_parent);
        exit(1);
    }

    // Fork to create CHILD WORKER
    child_pid = fork();
    if (child_pid == -1) {
        perror("PARENT WORKER fork");
        close(pipe_boss_to_worker_parent);
        close(pipe_workers[0]);
        close(pipe_workers[1]);
        exit(1);
    }

    if (child_pid == 0) { // CHILD WORKER
        close(pipe_boss_to_worker_parent);         // BOSS pipe not needed
        close(pipe_workers[1]);  // don't need to write to parent 
        
        child_worker_run(pipe_workers[0]); 
    
    } else { // PARENT WORKER
        close(pipe_workers[0]); // don't need to read from child
        
        char byte_in, byte_out;

        // Read from BOSS, process, write to CHILD WORKER
        while (read(pipe_boss_to_worker_parent, &byte_in, sizeof(byte_in)) > 0) {
            printf("PARENT WORKER: Received byte %d from BOSS\n", byte_in);
            
            byte_out = byte_in * byte_in;

            printf("PARENT WORKER: Sending byte %d to CHILD WORKER\n", byte_out);
            if (write(pipe_workers[1], &byte_out, sizeof(byte_out)) == -1) {
                perror("PARENT WORKER write");
                break; // Exit loop on error
            }
        }

        // Clean up
        printf("PARENT WORKER: Pipe from BOSS closed. Waiting for CHILD WORKER to exit...\n");
        close(pipe_boss_to_worker_parent);        // Close read end from BOSS
        close(pipe_workers[1]); // Close write end to CHILD WORKER (sends EOF)
        
        wait(NULL); // Wait for CHILD WORKER to finish
        printf("PARENT WORKER: CHILD WORKER finished. Exiting.\n");
    }

    return 0;
}