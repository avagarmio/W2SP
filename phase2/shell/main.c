#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_ARGS 64


enum redir { 
    //REDIR_NONE,
    REDIR_OUT = O_WRONLY | O_CREAT | O_TRUNC, 
    REDIR_APPEND = O_WRONLY | O_CREAT | O_APPEND, 
    REDIR_ERR = O_WRONLY | O_CREAT | O_TRUNC, 
    REDIR_IN = O_RDONLY
};


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


static int redirect(const char *path, int flags, int change_fd){
    int fd = open(path, flags, 0644);
    if(fd == -1) return fd;
    if (dup2(fd, change_fd) == -1){
        close(fd);
        return -1;
    }
    if(fd != change_fd) close(fd);
    return 0;
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

        char *stdout_path = NULL;
        char *stdappend_path = NULL;
        char *stderr_path = NULL;
        char *stdin_path = NULL;

        bool near = true;

        for (int i = 0; i < argc; ++i) {
            char *op = args[i];

            if (strcmp(op, ">") != 0 && strcmp(op, ">>") != 0 &&
                strcmp(op, "<") != 0 && strcmp(op, "2>") != 0)
                continue;

            if (i + 1 >= argc) {
                fprintf(stderr, "w2sp: syntax error near '%s'\n", op);
                near = false;
                break;
            }

            // char *path = args[i + 1];
            args[i] = NULL;

            if (strcmp(op, ">") == 0) {
                stdout_path = args[i + 1];
                continue;
            } else if(strcmp(op, ">>") == 0){
                stdappend_path = args[i + 1];
                continue;
            } else if(strcmp(op, "2>") == 0){
                stderr_path = args[i + 1];
                continue;
            } else if(strcmp(op, "<") == 0){
                stdin_path = args[i + 1];
                continue;
            }
        }

        if(!near){
            continue;
        }

        if(args[0] == NULL){
            fprintf(stderr, "w2sp: syntax error: missing command\n");
            continue;
        }


        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        }

        if (pid == 0) {

            // if(operator_type != 0){
            //     int rc = redirect(redir_path, operator_type);
                
            // }
            
            if(stdout_path != NULL){
                int rc = redirect(stdout_path, REDIR_OUT, STDOUT_FILENO);
                if(rc == -1){
                    perror("redirect");
                    exit(1);
                }
            }
            if(stdappend_path != NULL){
                int rc = redirect(stdappend_path, REDIR_APPEND, STDOUT_FILENO);
                if(rc == -1){
                    perror("redirect");
                    exit(1);
                }
            }
            if(stderr_path != NULL){
                int rc = redirect(stderr_path, REDIR_ERR, STDERR_FILENO);
                if(rc == -1){
                    perror("redirect");
                    exit(1);
                }
            }
            if(stdin_path != NULL){
                int rc = redirect(stdin_path, REDIR_IN, STDIN_FILENO);
                if(rc == -1){
                    perror("redirect");
                    exit(1);
                }
            }

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