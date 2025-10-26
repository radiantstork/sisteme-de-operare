#include <unistd.h>
#include <stdio.h>
#include <string.h>

// QUESTION:
// Why is the printing ordering like it is?
// Hint: Read is a blocking operation

// What are some mistakes here/what can be improved?

int main(){
    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];
    char nr;
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1){
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
        close(pipe_parent_to_child[1]); // Close write fd
        close(pipe_child_to_parent[0]); // Close read fd

        // Receive from parent
        while (read(pipe_parent_to_child[0], &nr, 1) > 0){
            printf("CHILD: Received byte %d from parent\n", nr);
            
            // Send to parent
            nr = nr + 1;
            printf("CHILD: Sending byte %d to parent\n", nr);
            write(pipe_child_to_parent[1], &nr, 1);
        }
        close(pipe_parent_to_child[0]); // Close read fd
        close(pipe_child_to_parent[1]); // Close write fd 
        return 0;
    }
    // Parent executes
    else{
        close(pipe_parent_to_child[0]); // Close read fd
        close(pipe_child_to_parent[1]); // Close write fd

        for (char c = 0; c < 5; ++c){
            // Send to child
            printf("PARENT: Sending byte %d to child\n", c);
            write(pipe_parent_to_child[1], &c, 1);
            
            // Receive from child
            read(pipe_child_to_parent[0], &nr, 1);
            printf("PARENT: Received byte from child %d\n", nr);
        }

        close(pipe_parent_to_child[1]); // Close write fd
        close(pipe_child_to_parent[0]); // Close read fd

        wait(NULL);
        return 0;
    }
}