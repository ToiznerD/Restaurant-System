#define MAX 256
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char** build_argv(char* buffer, int* bin, int* args_c){
    //Build argv[]
    char* token, **arg;
    int i=0;
    *args_c = 0;
    token = strtok(buffer, " ");//token = first cut of argv[1] string untill ' '
    arg = (char**)malloc(sizeof(char*)*(i+1));
    if(arg == NULL){ perror("malloc"); exit(1); }
    while(token != NULL){
        arg[i] = token; //insert token to arg
        (*args_c)++;
        token = strtok(NULL, " "); //next word
        i++;
        arg = (char**)realloc(arg, sizeof(char*)*(i+1));
        if(arg == NULL){ perror("malloc"); exit(1); }
    }
    arg[i] = NULL;
    if(strcmp(arg[0],"CreateMenu") && strcmp(arg[0],"getMenu") && strcmp(arg[0],"MakeOrder") && strcmp(arg[0],"getPrice") && strcmp(arg[0],"getOrderNum")){
        *bin = 1;
    } else *bin = 0;
    return arg;
}

int getCommand(char*** arg,char* buffer, int* bin){
    int args_c;
    printf("AdvShell>");
    fgets(buffer, MAX, stdin);
    buffer[strlen(buffer)-1] = '\0';
    while(strlen(buffer) == 0){ //Check for blank cmd
        printf("AdvShell>");
        fgets(buffer, MAX, stdin);
        buffer[strlen(buffer)-1] = '\0';
    }
    *arg = build_argv(buffer, bin, &args_c);
    return args_c;
}

int main(int argc, char* argv[]){
    if(argc != 1){
        printf("wrong input?");
        exit(1);
    }
    char buffer[MAX] = {0};
    char** arg;
    int pid, i=0, bin=0;
    while(getCommand(&arg, buffer, &bin) > 4) printf("I don't support more than 3 arguments.\n");
    while(strcmp(buffer,"exit") && strcmp(buffer,"Exit")){
        if((pid = fork()) == 0){
            //Run binary cmd?
            if(bin == 0) execv(*arg,arg);
            else execvp(*arg, arg);
            //perror("exec"); Unnecessary?
        }
        else if(pid == -1){
            perror("fork");
            exit(1);
        }
        if(pid == 0){
            printf("Not Supported\n");
            exit(1);
        }
        wait();
        while(getCommand(&arg, buffer, &bin) > 4) printf("I don't support more than 3 arguments.\n");
    }
    printf("Goodbye.\n");
    return 0;
}