#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void main(int argc, char* argv[]){
    if(argc != 2){
        printf("not enough arguments.\n");
        exit(1);
    }
    char* arg[] = {"cat",argv[1],NULL};
    execvp(arg[0],arg);
    perror("exec");
}