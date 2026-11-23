#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>



int main(void){
    int pipefd[2];

    if(pipe(pipefd) == -1){
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if(pid == 0){
        close(pipefd[1]);

        char buf[128];
        
        ssize_t o = read(pipefd[0], buf, sizeof(buf));
        if(o == -1){
            perror("read");
            exit(1);
        }

        ssize_t read_count = 0;
        while(o != 0){
            o = read(pipefd[0], buf, 19);
            read_count += o;
            if(o == -1){
                perror("read");
                exit(1);
            }
        }

        if(read_count < 128) buf[read_count] = '\0';

        printf("Child: %s\n", buf);

        exit(0);

    } else{
        close(pipefd[0]);

        const char *msg = "hello from parrent\n";

        if(write(pipefd[1], msg, strlen(msg)) == -1){
            close(pipefd[1]);
            perror("write");
            return 1;
        }

        close(pipefd[1]);

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(status))
            printf("Exited with code %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Killed by signal %d\n", WTERMSIG(status));
    }
}