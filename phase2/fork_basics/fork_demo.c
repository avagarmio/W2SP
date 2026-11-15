#include <unistd.h>
#include <stdio.h>


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
    } else{
        printf("Parent: pid = %ld, child pid = %ld\n", (long)getpid(), (long)pid);
    }


    return 0;
}