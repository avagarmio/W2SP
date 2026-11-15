#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
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
        sleep(3);
        printf("Child: pid = %ld, ppid = %ld\n", (long)getpid(), (long)getppid());
        
    } else{
        printf("parent exiting\n");
    }


    return 0;
}