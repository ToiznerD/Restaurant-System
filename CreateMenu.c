#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define MAX 256


char** build_dish(char* buffer, int* str_ctr){
    //Build dish
    char* token, **arg;
    int i=0;
    token = strtok(buffer, " ");//token = first cut of buffer string untill ' '
    arg = (char**)malloc(sizeof(char*)*(i+1));
    if(arg == NULL){ perror("allocation"); exit(1); }
    while(token != NULL){
         arg[i] = token; //insert token to arg
         *str_ctr += 1;
         token = strtok(NULL, " "); //next word
         i++;
         if(token != NULL){
            arg = (char**)realloc(arg, sizeof(char*)*(i+1));
            if(arg == NULL){ perror("allocation"); exit(1); }
         }
    }
    return arg;
}


int main(int argc, char* argv[]){
    if(argc != 3){
        printf("not enough arguments.\n");
        exit(1);
    }
    int dish_num = 1, fd_to, wbytes, i, j=0, r, dots_num, dishLen=0;
    char buffer[MAX], cToStr[2],  dish_type = 'a';
    char* temp, **dish, *dots = (char*)malloc(sizeof(char));
    if(dots == NULL){ perror("allocation"); exit(1); }
    cToStr[1] = '\0'; //string to convert dish_type into string

    //Create res_order dir
    temp = (char*)malloc(sizeof(char)*(strlen(argv[1])+7));
    if(temp == NULL){  perror("allocation"); exit(1); }
    strcat(temp,argv[1]);
    strcat(temp,"_Order");
    if(mkdir(temp,0777) != 0){
        perror("mkdir");
        return -1;
    }

    //Create menu file
    if( ( fd_to = open( argv[1], O_WRONLY | O_CREAT, 0666 ) ) == -1 ){
        perror( "open to" );
        return -1;
    }

    //Menu title
    if( ( wbytes = write( fd_to, argv[1], strlen(argv[1]) ) ) == -1 )
			{ perror( "write" ); return -1; }
    if( ( wbytes = write( fd_to, " Menu\n\n", 7 ) ) == -1 )
			{ perror( "write" ); return -1; }
    
    //Get dishes
    for(i = 0;i<atoi(argv[2]);i++){
        printf("Insert type dish %c:\n", dish_type);
        fgets(buffer, MAX, stdin);
        buffer[strlen(buffer)-1] = '\0';
        //Write type dish to menu
        temp = (char*)malloc(sizeof(char)*(strlen(buffer)+6));
        if(temp == NULL){ perror("allocation"); exit(1); }
        cToStr[0] = dish_type;
        strcat(temp,"\n");
        strcat(temp,cToStr);
        strcat(temp,". ");
        strcat(temp,buffer);
        strcat(temp,"\n");
        if( ( wbytes = write( fd_to,temp, strlen(temp) ) ) == -1 )
			{ perror( "write" ); return( -1 ); }
        
        //Receive dishes
        printf("Insert dish name %d:\n", dish_num);
        fgets(buffer, MAX, stdin);
        buffer[strlen(buffer)-1] = '\0';
        temp = realloc(temp,sizeof(char)*50);
        if(temp == NULL){ perror("allocation");  exit(1);   }
        while(strcmp(buffer,"Stop")){
            dish = build_dish(buffer, &dishLen);

            //build dish line to write
            for(j = 0;j<50;j++)
                temp[j] = 0;
            strcat(temp,"   ");
            for(j=0;j<dishLen;j++){
                if(j == dishLen-1 && atoi(dish[j]) != 0){
                    //Price
                    dots_num = 49-dots_num-7-strlen(dish[j])-dishLen;
                    for(r = 0;r<dots_num;r++)
                        strcat(temp,".");
                    strcat(temp," ");
                    strcat(temp,dish[j]);
                    strcat(temp,"NIS\n");
                    break;
                }
                else if(j == dishLen-1){
                    //Free dish
                    dots_num += strlen(dish[j]);
                    strcat(temp,dish[j]);
                    strcat(temp," ");
                    dots_num = 49-dots_num-9-dishLen;
                    for(r = 0;r<dots_num;r++)
                        strcat(temp,".");
                    strcat(temp," 0");
                    strcat(temp,"NIS\n");
                    break;
                }
                dots_num += strlen(dish[j]);
                strcat(temp,dish[j]);
                strcat(temp," ");
            }

            //Write dish line into menu
            if( ( wbytes = write( fd_to,temp, strlen(temp) ) ) == -1 )
			{ perror( "write" ); return( -1 ); }

            //Restart
            dish_num++;
            printf("Insert dish name %d:\n", dish_num);
            fgets(buffer, MAX, stdin);
            buffer[strlen(buffer)-1] = '\0';
            dishLen = 0;
            dots_num = 0;
        }

        //Prepare for next dish type
        dish_num = 1;
        dish_type++;
    }
    
    //Add Bon Appetit
    if( ( wbytes = write( fd_to,"\n\n\n\t\t\tBon Appetit\n", 18 ) ) == -1 )
    { perror( "write" ); return( -1 ); }
    close(fd_to);
    free(temp);
    free(dots);
    for(i=0;i<dishLen;i++)
        free(dish[i]);
    free(dish);
    printf("\nSuccessfuly created.\n");
    return 0;
}