#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_ARGS 64


enum redir {
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
        int saved_errno = errno;
        close(fd);
        errno = saved_errno;
        return -1;
    }
    if(fd != change_fd) close(fd);
    return 0;
}


static int wait_child(pid_t pid, int *status, const char *message){
    pid_t result;

    do {
        result = waitpid(pid, status, 0);
    } while (result == -1 && errno == EINTR);

    if (result == -1){
        perror(message);
        return -1;
    }
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

        int pipe_pos = -1;
        bool invalid_pipe = false;

        for (int i = 0; i < argc; ++i) {
            if (strcmp("|", args[i]) != 0)
                continue;

            if (i == 0 || i == argc - 1 || pipe_pos != -1) {
                fprintf(stderr, "w2sp: invalid or unsupported pipeline\n");
                invalid_pipe = true;
                break;
            }

            pipe_pos = i;
        }

        if (invalid_pipe)
            continue;

        if (pipe_pos != -1) {
            args[pipe_pos] = NULL;
            char **right_command = &args[pipe_pos + 1];

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("w2sp: pipe");
                continue;
            }

            pid_t pid_left = fork();
            if (pid_left == -1) {
                perror("w2sp: fork left");
                close(pipefd[0]);
                close(pipefd[1]);
                continue;
            }

            if (pid_left == 0) {
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("w2sp: dup2 stdout");
                    _exit(1);
                }
                if (pipefd[0] != STDOUT_FILENO)
                    close(pipefd[0]);
                if (pipefd[1] != STDOUT_FILENO)
                    close(pipefd[1]);

                execvp(args[0], args);
                perror("w2sp: execvp left");
                _exit(127);
            }

            pid_t pid_right = fork();
            if (pid_right == -1) {
                perror("w2sp: fork right");
                close(pipefd[0]);
                close(pipefd[1]);
                wait_child(pid_left, NULL, "w2sp: waitpid left");
                continue;
            }

            if (pid_right == 0) {
                if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                    perror("w2sp: dup2 stdin");
                    _exit(1);
                }
                if (pipefd[0] != STDIN_FILENO)
                    close(pipefd[0]);
                if (pipefd[1] != STDIN_FILENO)
                    close(pipefd[1]);

                execvp(right_command[0], right_command);
                perror("w2sp: execvp right");
                _exit(127);
            }

            close(pipefd[0]);
            close(pipefd[1]);

            int status_left;
            int status_right;
            int left_result = wait_child(pid_left, &status_left, "w2sp: waitpid left");
            int right_result = wait_child(pid_right, &status_right, "w2sp: waitpid right");

            if (left_result == 0 && WIFSIGNALED(status_left))
                printf("Left command killed by signal %d\n", WTERMSIG(status_left));
            if (right_result == 0 && WIFSIGNALED(status_right))
                printf("Right command killed by signal %d\n", WTERMSIG(status_right));

            continue;
        }

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
        } else if(strcmp("export", args[0]) == 0){
            if(args[1] == NULL) {
                fprintf(stderr, "w2sp: export: usage: export NAME=value\n");
                continue;
            }

            for(size_t i = 1; args[i] != NULL; ++i){
                char *eq = strchr(args[i], '=');

                if(eq == NULL){
                    fprintf(stderr, "w2sp: export: <%s> has no value. Usage: export NAME=value\n", args[i]);
                    continue;
                }

                if(args[i] == eq){
                    fprintf(stderr, "w2sp: export: <%s> has no name. Usage: export NAME=value\n", args[i]);
                    continue;
                }

                *eq = '\0';

                if(setenv(args[i], eq+1, 1) == -1){
                    perror("w2sp: export");
                    continue;
                }
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
            if(stdout_path != NULL){
                int rc = redirect(stdout_path, REDIR_OUT, STDOUT_FILENO);
                if(rc == -1){
                    perror("redirect");
                    _exit(1);
                }
            }
            if(stdappend_path != NULL){
                int rc = redirect(stdappend_path, REDIR_APPEND, STDOUT_FILENO);
                if(rc == -1){
                    perror("redirect");
                    _exit(1);
                }
            }
            if(stderr_path != NULL){
                int rc = redirect(stderr_path, REDIR_ERR, STDERR_FILENO);
                if(rc == -1){
                    perror("redirect");
                    _exit(1);
                }
            }
            if(stdin_path != NULL){
                int rc = redirect(stdin_path, REDIR_IN, STDIN_FILENO);
                if(rc == -1){
                    perror("redirect");
                    _exit(1);
                }
            }

            execvp(args[0], args);
            perror("execvp");
            _exit(127);
        }

        int status;
        if (wait_child(pid, &status, "waitpid") == -1) {
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
