#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;

    printf("Parent Process (PID: %d) started.\n", getpid());

    pid1 = fork();

    if (pid1 < 0) {
        perror("Fork failed for child 1");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        
        printf("Child 1 (PID: %d, Parent PID: %d): Displaying Date\n", getpid(), getppid());
        execlp("date", "date", NULL);
        perror("execlp failed for date");
        exit(EXIT_FAILURE);
    }

    pid2 = fork();

    if (pid2 < 0) {
        perror("Fork failed for child 2");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
       
        printf("Child 2 (PID: %d, Parent PID: %d): Listing Directory Contents\n", getpid(), getppid());
        execlp("ls", "ls", NULL);
        perror("execlp failed for ls"); 
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    wait(NULL);

    printf("Parent Process (PID: %d) exiting after children completed.\n", getpid());
    
    return 0;
}
