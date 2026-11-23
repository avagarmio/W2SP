#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

extern char **environ;


int main(void){
    for(int i = 0; environ[i] != NULL; ++i){
        puts(environ[i]);
    }
    
    return 0;
}
