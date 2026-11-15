#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>


int main(void){
    printf("Before: %ld\n", (long)getpid());

    fflush(stdout);

    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return 1;
    }

    if(pid == 0){
        printf("Child: pid = %ld, ppid = %ld\n", (long)getpid(), (long)getppid());
        int *p = NULL;
        *p = 42;
        exit(300);
    } else{
        printf("Parent: pid = %ld, child pid = %ld\n", (long)getpid(), (long)pid);

        int status;
        pid_t returned_pid = waitpid(pid, &status, 0);
        if(returned_pid == -1){
            perror("waitpid");
            return 1;
        }

        if(WIFEXITED(status)){
            printf("child %ld exited with code %d\n", (long)pid, WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)){
            printf("child %ld killed by signal %d\n", (long)pid, WTERMSIG(status));
        }
    }


    return 0;
}