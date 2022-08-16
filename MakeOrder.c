#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#define MAX 256

char** build_dish(char* buffer, int* str_ctr);
int getOrder(int fd_to, char* menu, int* bill);
int update_bill(char* buffer, char* menu, int* bill);

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Not enough arguments.\n");
        exit(1);
    }
    char path[MAX] = "", buffer[MAX] = "", menu[1024] = "";
    int fd_to, fd_menu, wbytes, rbytes, bill = 0;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    //Open menu
    if( ( fd_menu = open(argv[1], O_RDONLY) ) == -1 ){
        printf("Restaurant Not Found!");
        return -1;
    }

    //Build path & Create order file
    strcat(path,argv[1]);
    strcat(path,"_Order/");
    strcat(path,argv[2]);
    if( ( fd_to = open(path, O_WRONLY | O_CREAT, 0444 ) ) == -1 ){
        perror( "open to" );
        return -1;
    }

    //Write title
    strcat(buffer,argv[1]);
    strcat(buffer," Order\n\n");
    if( ( wbytes = write( fd_to,buffer, strlen(buffer) ) ) == -1 )
    { perror( "write" ); return( -1 ); }


    //Read menu
    if((rbytes = read(fd_menu,menu,1024)) == -1){
        perror("read from");
        return -1;
    }

    //Get order
    getOrder(fd_to,menu,&bill);

    //Sum up order
    printf("Total Price: %d NIS (Confirm to approve/else cancel)\n", bill);
    fgets(buffer, MAX, stdin);
    buffer[strlen(buffer)-1] = '\0';
    while(!strcmp(buffer,"Cancel") || !strcmp(buffer,"cancel")){
        //The user entered 'cancel'
        getOrder(fd_to,menu,&bill);
        printf("Total Price: %d NIS (Confirm to approve/else cancel)\n", bill);
        fgets(buffer, MAX, stdin);
        buffer[strlen(buffer)-1] = '\0';
    }
    while(strcmp(buffer,"Confirm") && strcmp(buffer,"confirm")){
        //Illegal input
        printf("Wrong input (Confirm to approve/else cancel):\n");
        fgets(buffer, MAX, stdin);
        buffer[strlen(buffer)-1] = '\0';
        while(!strcmp(buffer,"Cancel") || !strcmp(buffer,"cancel")){
            //The user entered 'cancel'
            getOrder(fd_to,menu,&bill);
            printf("Total Price: %d NIS (Confirm to approve/else cancel)\n", bill);
            fgets(buffer, MAX, stdin);
            buffer[strlen(buffer)-1] = '\0';
        }
    }

    //Write total price & date
    strcpy(buffer,"");
    sprintf(buffer,"Total Price: %dNIS\n\n%02d/%02d/%d", bill, tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900);
    if( ( wbytes = write( fd_to,buffer, strlen(buffer) ) ) == -1 )
    { perror( "write" ); return( -1 ); }

    printf("\nOrder Created!\n");
    close(fd_to);
    close(fd_menu);
    return 0;

}

char** build_dish(char* buffer, int* str_ctr){
    //Build dish
    char* token, **arg;
    int i=0;
    token = strtok(buffer, " ");//token = first cut of buffer string untill ' '
    arg = (char**)malloc(sizeof(char*)*(i+1));
    if (arg == NULL) { perror("allocation"); exit(1); }
    while(token != NULL){
         arg[i] = token; //insert token to arg
         token = strtok(NULL, " "); //next word
         i++;
         if(token != NULL){
            *str_ctr += 1;
            arg = (char**)realloc(arg, sizeof(char*)*(i+1));
            if (arg == NULL) { perror("allocation"); exit(1); }
         }
    }
    return arg;
}


int getOrder(int fd_to, char* menu, int* bill){
    //Get dishes & amount to order
    char buffer[MAX] = "";
    int wbytes;
    //Get dish & amount
    printf("Insert your order(Finish to finish):\n");
    fgets(buffer, MAX, stdin);
    buffer[strlen(buffer)-1] = '\0';
    while(strcmp(buffer,"Finish") && update_bill(buffer,menu,bill) != 0 ){
            printf("\n(#1)Wrong dish. try again: ");
            fgets(buffer, MAX, stdin);
            buffer[strlen(buffer)-1] = '\0';
    }

    while(strcmp(buffer,"Finish")){
        //Write dish & amount to file
        strcat(buffer,"\n");
        if( ( wbytes = write( fd_to,buffer, strlen(buffer) ) ) == -1 )
        { perror( "write" ); return( -1 ); }
        //Get dish & amount
        fgets(buffer, MAX, stdin);
        buffer[strlen(buffer)-1] = '\0';
        while(strcmp(buffer,"Finish") && update_bill(buffer,menu,bill) != 0 ){
            printf("\n(#2)Wrong dish. try again: ");
            fgets(buffer, MAX, stdin);
            buffer[strlen(buffer)-1] = '\0';
        }
    }
    return 0;
}

int update_bill(char* buffer, char* menu, int* bill){
    //Update the total order bill
    int order_len = 1, i=0, j=1;
    char** dish_line, *buffer_copy = (char*)malloc(sizeof(char)*strlen(buffer)), *dish_spot, *price = (char*)malloc(sizeof(char)*2), temp_dish[256] = "";
    if(price == NULL) { perror("allocation"); exit(1); }
    if(buffer_copy == NULL) { perror("allocation"); exit(1); }
    strcpy(buffer_copy,buffer);
    dish_line = build_dish(buffer_copy, &order_len);

    if(order_len < 2) //wrong input
        return -1;
    else if(order_len > 2){ //if dish name is longer than 1 word
        i=0;
        while(i<order_len-1){
            strcat(temp_dish,dish_line[i]);
            strcat(temp_dish," ");
            i++;
        }
    }
    else strcat(temp_dish,dish_line[0]);
    
    //Locate dish in the menu
    dish_spot = strstr(menu,temp_dish);
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
        price = realloc(price,sizeof(char)*(j+1)); if(price == NULL){perror("Allocation"); exit(1);}
        price[j-1] = dish_spot[i];
        j++;
        i++;
    }
    price[j] = '\0';
    
    //Update bill
    *bill += atoi(price)*atoi(dish_line[order_len-1]);

    free(buffer_copy);
    free(price);
    return 0;
}
