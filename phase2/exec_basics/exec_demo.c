#include <stdio.h>
#include <unistd.h>


int main(){
    printf("exec_demo: pid=%ld\n", (long)getpid());
    
    char *args[] = {"ps", "-o", "pid,ppid,cmd", NULL};
    execvp(args[0], args);

    perror("execvp");
    perror("execvp");
    printf("after exec\n");
    return 1;
}