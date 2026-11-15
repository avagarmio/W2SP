#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 64

static int parse_line(char *line, char *args[], int max_args)
{
    const char *delim = " \t";
    int count = 0;

    char *s = strtok(line, delim);
    while (s != NULL) {
        if (count == max_args)
            return -1;
        args[count++] = s;
        s = strtok(NULL, delim);
    }
    args[count] = NULL;
    return count;
}

int main(void)
{
    char *commands = NULL;
    size_t capacity = 0;

    while (1) {
        printf("w2sp$ ");
        fflush(stdout);

        ssize_t len = getline(&commands, &capacity, stdin);
        if (len == -1)
            break;
        if (len > 0 && commands[len - 1] == '\n')
            commands[len - 1] = '\0';

        char *args[MAX_ARGS + 1];
        int argc = parse_line(commands, args, MAX_ARGS);
        if (argc == -1) {
            fprintf(stderr, "w2sp: too many arguments\n");
            continue;
        }
        if (argc == 0)
            continue;

        if(strcmp("exit", args[0]) == 0){
            break;
        } else if(strcmp("cd", args[0]) == 0){
            char *path = (args[1] == NULL) ? getenv("HOME") : args[1];
            
            if(path == NULL){
                fprintf(stderr, "w2sp: cd: HOME not set\n");
                continue;
            }

            if(chdir(path) == -1){
                perror("w2sp: cd");
            }
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp");
            exit(127);
        }

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status))
            printf("Exited with code %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Killed by signal %d\n", WTERMSIG(status));
    }

    free(commands);
    printf("\n");
    return 0;
}