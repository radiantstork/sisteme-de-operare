#include <stdio.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <stdlib.h>

// We fork in order to create a child process, which exits with code 0, but the
// parent, because is sleeping, doesn't see the exit signal of the child,
// so it remains a zombie



int main(){
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    
	
    /* Child */
    if (pid == 0){
    	printf("Id of child: %d\n", getpid());
    	printf("Id of parent %d", getppid());
        exit(0); // the child ends it's exexcution
	}
        

    sleep(5); // the parent sleeps for 5 seconds, so it doesn't see the 
    		// exit code of the child

    return 0;
}
