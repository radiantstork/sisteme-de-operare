#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// Define pipe names - must be consistent across processes
const char *pipe_boss_to_worker_parent_name = "/tmp/pipe_boss_to_worker_parent";
const char *pipe_worker_child_to_boss_name = "/tmp/pipe_worker_child_to_boss";

void cleanup_and_exit(int fd1, int fd2, int exit_code) {
    if (fd1 != -1) close(fd1);
    if (fd2 != -1) close(fd2);
    
    unlink(pipe_boss_to_worker_parent_name);
    unlink(pipe_worker_child_to_boss_name);
    exit(exit_code);
}

int main() {
    // File descriptors of pipes    
    int pipe_boss_to_worker_parent = -1;
    int pipe_worker_child_to_boss = -1;
    
    // Create the named pipes (FIFOs)
    // S_IRUSR | S_IWUSR = 0600 (user read/write)
    if (mkfifo(pipe_boss_to_worker_parent_name, S_IRUSR | S_IWUSR) == -1) {
        perror("mkfifo pipe_boss_to_worker_parent");
    }
    if (mkfifo(pipe_worker_child_to_boss_name, S_IRUSR | S_IWUSR) == -1) {
        perror("mkfifo pipe_worker_child_to_boss");
    }

    printf("BOSS: Waiting to open pipes...\n");
    
    // Open pipe to PARENT WORKER for writing.
    // This will block until PARENT WORKER opens it for reading.
    pipe_boss_to_worker_parent = open(pipe_boss_to_worker_parent_name, O_WRONLY);
    if (pipe_boss_to_worker_parent == -1) {
        perror("BOSS open pipe_boss_to_worker_parent for writing");
        cleanup_and_exit(pipe_boss_to_worker_parent, pipe_worker_child_to_boss, 1);
    }

    // Open pipe from CHILD WORKER for reading.
    // This will block until CHILD WORKER opens it for writing.
    pipe_worker_child_to_boss = open(pipe_worker_child_to_boss_name, O_RDONLY);
    if (pipe_worker_child_to_boss == -1) {
        perror("BOSS open pipe_worker_child_to_boss for reading");
        cleanup_and_exit(pipe_boss_to_worker_parent, pipe_worker_child_to_boss, 1);
    }
    printf("BOSS: All pipes connected.\n");

    // Send data to worker parent
    for (char i = 0; i <= 5; ++i) {
        printf("BOSS: Sending byte %d to PARENT WORKER\n", i);
        if (write(pipe_boss_to_worker_parent, &i, sizeof(i)) == -1) {
            perror("BOSS write");
            cleanup_and_exit(pipe_boss_to_worker_parent, pipe_worker_child_to_boss, 1);
        }
    }
    
    // Close our write end. This sends an End-of-File (EOF) signal
    // to PARENT WORKER, so it knows we're done sending.
    close(pipe_boss_to_worker_parent);
    pipe_boss_to_worker_parent = -1; // Mark as closed
    printf("BOSS: Finished sending. Waiting for final results...\n");

    // Receive final results from CHILD WORKER
    char result_byte;
    for (int i = 0; i <= 5; ++i) {
        ssize_t bytes_read = read(pipe_worker_child_to_boss, &result_byte, sizeof(result_byte));
        
        if (bytes_read == -1) {
            perror("BOSS read");
            cleanup_and_exit(pipe_boss_to_worker_parent, pipe_worker_child_to_boss, 1);
        } else if (bytes_read == 0) {
            // This means CHILD WORKER closed its end of the pipe
            printf("BOSS: Pipe from CHILD WORKER closed early!\n");
            break;
        }
        
        // Use %d to print the numerical value of the char
        printf("BOSS: <<< Received final result: %d\n", result_byte);
    }

    // Clean up
    printf("BOSS: All results received. Cleaning up and exiting.\n");
    cleanup_and_exit(pipe_boss_to_worker_parent, pipe_worker_child_to_boss, 0);
    
    return 0; // Unreachable
}
