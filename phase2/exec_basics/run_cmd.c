#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void){
    char *commands = NULL;
    size_t capacity = 0;

    while(1){
        printf("w2sp$ ");
        fflush(stdout);
        ssize_t len = 0;

        if((len = getline(&commands, &capacity, stdin)) == -1) break;
        if(len > 0 && commands[len - 1] == '\n'){
            commands[len-1] = '\0';
        }

        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            return 1;
        }

        if(pid == 0){
            char *args[] = {commands, NULL};
            execvp(args[0], args);
            perror("execvp");
            exit(127);
        } else{
            int status;
            pid_t returned_pid = waitpid(pid, &status, 0);
            if(returned_pid == -1){
                perror("waitpid");
                continue;
            }

            if(WIFEXITED(status)){
                printf("Exited with code %d\n", WEXITSTATUS(status));
            } else if(WIFSIGNALED(status)){
                printf("Killed by signal %d\n", WTERMSIG(status));
            }
        }
    }
    free(commands);

    printf("\n");
    return 0;

} 
