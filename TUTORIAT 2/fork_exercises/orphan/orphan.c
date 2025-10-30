#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// The parent ends it's execution before the child process (because it is
// sleeping). The child becomes an orphan process and it gets "adopted"
// by the "init" process, which will eventually call wait for it


int main()
{	
    if(!fork()){
    	printf("Id of child from child %d\n", getpid());
    	printf("Id of parent from child %d\n", getppid());
        sleep(10);
        printf("Id of parent from child %d\n", getppid());
        printf("Where are you father?\n");
        }
    else{
    	sleep(1);
    	printf("Id of parent from parent %d\n", getpid());
    	printf("Father terminated\n");
    }
    return 0;
}
