#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#define MAX 256

int update_bill(char* buffer, char* menu, int* bill){
    //Update the total bill
    int order_len = 1, i=0, j=1;
    char *buffer_copy = (char*)malloc(sizeof(char)*strlen(buffer)), *dish_spot, *price = (char*)malloc(sizeof(char)*2);
    if(price == NULL){ perror("Allocation"); exit(1); } 
    if(buffer_copy == NULL){ perror("Allocation"); exit(1); }
    strcpy(buffer_copy,buffer);
    dish_spot = strstr(menu,buffer_copy);
    if(dish_spot == NULL){
        return -1;
    }

    //Search price
    i=1;
    if(!(dish_spot[i-2] == ' ' && dish_spot[i-3] == ' '))
        return -1;
    if(dish_spot[strlen(buffer_copy)+1] != '.')
        return -1;
    while(!isdigit(dish_spot[i])){
        i++;
    }
    while(isdigit(dish_spot[i])){
        price = realloc(price,sizeof(char)*(j+1)); if(price == NULL){ perror("Allocation"); exit(1); }
        price[j-1] = dish_spot[i];
        j++;
        i++;
    }
    price[j] = '\0';

    //Update bill
    *bill += atoi(price);

    free(price);
    free(buffer_copy);
    return 0;
}


int main(int argc, char* argv[]){
    if (argc < 2){
        printf("Not enough arguments.\n");
        exit(1);
    }
    char dish[MAX] = "", menu[1024] = "";
    int fd_menu, bill, rbytes, i;
    if(argc >= 4){
        //Dish name is longer than 1 word
        for(i=2;i<argc;i++){
            strcat(dish,argv[i]);
            if(i != argc-1)
                strcat(dish," ");
        }
    }
    else strcpy(dish,argv[2]);
    

    //Open menu
    if( ( fd_menu = open(argv[1], O_RDONLY) ) == -1 ){
        printf("Restaurant Not Found!\n");
        return -1;
    }

    //Read menu
    if((rbytes = read(fd_menu,menu,1024)) == -1){
        perror("read from");
        return -1;
    }

    //Get price
    if(update_bill(dish,menu, &bill) != 0){
        printf("Dish not found!\n");
        exit(1);
    }
    printf("%d NIS\n", bill);
    return 0;

}