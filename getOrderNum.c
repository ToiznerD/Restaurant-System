#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#define MAX 256

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Wrong input.\n");
        exit(1);
    }
    int fd_menu, file_count = 0;
    char path[MAX] = "";
    DIR * dirp;
    struct dirent * entry;

    //Open menu
    if( ( fd_menu = open(argv[1], O_RDONLY) ) == -1 ){
        printf("Restaurant Not Found!\n");
        return -1;
    }
    strcat(path,argv[1]);
    strcat(path,"_Order");
    //Open directory
    if((dirp = opendir(path)) == NULL){
        printf("NULL");
    }
    //Count files
    while ((entry = readdir(dirp)) != NULL) {
            file_count++;
    }
    closedir(dirp);
    printf("\n%d Orders.\n", file_count-2); //file_count include main directory & father directory
    close(fd_menu);
    return 0;

}