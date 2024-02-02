#pragma once

#include <stdio.h> 
#include <string.h> 
#include <syscall.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// #define Color_Red "\33[0:31m\\]" // Color Start
// #define Color_end "\33[0m\\]" // To flush out prev settings
#define LOG_RED(X) printf("%s %s %s",ANSI_COLOR_RED,X,ANSI_COLOR_RESET)
#define LOG_GREEN(X) printf("%s %s %s",ANSI_COLOR_GREEN,X,ANSI_COLOR_RESET)

typedef struct Account{
    char username[20];
    char password[20];
} Account;

char username[20];
int room_id;

int display_welcome_menu(){
    // system("clear");
    printf("---------------WELCOME TO AUCTION SYSTEM---------------\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("Please choose 1-2: ...\n");
    int n; 
    scanf("%d", &n);
    while(n != 1 && n != 2){
        printf("Please choose 1 or 2: ");
        scanf("%d", &n);
    }
    return n;
}

Account display_login_menu(){
    Account user_acc;
    printf("Please provide some information to login: \n");
    printf("Username: ");
    scanf("%s", user_acc.username);
    printf("Password: ");
    scanf("%s", user_acc.password);
    strcpy(username, user_acc.username);
    return user_acc;
}

Account display_register_menu(){
    Account user_acc;
    printf("Please provide some information to register: \n");
    printf("Username: ");
    scanf("%s", &user_acc.username);
    printf("Password: ");
    scanf("%s", &user_acc.password);
    return user_acc;
}

char* login_register_function(){
    int login_or_register = display_welcome_menu();
    Account sender_acc;
    char header[12];
    char data[100];
    char* sendline;
    if (login_or_register == 1){
        sender_acc = display_login_menu();
        sprintf(header, "LOGIN_REQ\0");
        sprintf(data, "username: %s; password: %s\0", sender_acc.username, sender_acc.password);
    }else {
        sender_acc = display_register_menu();
        sprintf(header, "REGIS_REQ\0");
        sprintf(data, "username: %s; password: %s\0", sender_acc.username, sender_acc.password);
    }

    sprintf(sendline, "HEADER: %s; DATA: %s", header, data);
    // printf("%s\n", sendline);
    // printf("%d\n", strlen(sendline));
    return sendline;
}

int choose_role(){
    system("clear");
    printf("---------------WELCOME TO AUCTION SYSTEM---------------\n");
    printf("1. Bidder\n");
    printf("2. Seller\n");
    printf("Please choose role 1-2: ...\n");
    int n; 
    scanf("%d", &n);
    while(n != 1 && n != 2){
        printf("Please choose 1 - 2: ");
        scanf("%d", &n);
    }    
    system("clear");
    return n;
}

int bidder_menu(){
    
    printf("---------------HELLO BIDDER---------------\n");
    printf("1. View all auction room\n");
    printf("2. Search auction item (Not working)\n");
    printf("3. Join room\n");
    printf("4. Back to choose role\n");
    printf("5. Exit\n");
    printf("Please choose 1-5: ...\n");
    int n; 
    scanf("%d", &n);
    while(n < 1 || n > 5){
        printf("Please choose from 1 to 5: ");
        scanf("%d", &n);
    }
    system("clear");
    return n;
}

int seller_menu(){
    
    printf("---------------HELLO SELLER---------------\n");
    printf("1. Create auction item\n");
    printf("2. View your items\n");
    printf("3. View all auction room\n");
    printf("4. Join room\n");
    printf("5. Create room\n");
    printf("6. Back to choose role\n");
    printf("7. Exit\n");
    printf("Please choose 1-7: ...\n");
    int n; 
    scanf("%d", &n);
    while(n < 1 || n > 7){
        printf("Please choose from 1 to 7: ");
        scanf("%d", &n);
    }
    system("clear");
    return n;
}

char* create_room(){
    printf("---------------CREATE ROOM---------------\n");
    printf("Please provide name to create room: \n");
    char room_name[30];
    scanf("%s", room_name);

    // printf("Room name: %s\n", room_name);

    printf("Do you want to create room %s? (y/n)\n", room_name);
    char confirm;
    scanf("%s", &confirm);
    while(confirm != 'y' && confirm != 'n'){
        printf("Please choose y or n: ");
        scanf("%c", &confirm);
    }
    if (confirm == 'y'){
        char* resultCopy = strdup(room_name);
        return resultCopy;
    }else{
        return "0";
    }
}

int bidder_in_room_menu(){
    // system("clear");
    printf("---------------IN ROOM %d---------------\n", room_id);
    printf("1. View all auction item in this room\n");
    printf("2. View Schedule\n");
    printf("3. Place a bid on an current auction item (Not working)\n");
    printf("4. Leave room\n");
    printf("Please choose 1-4: ...\n");
    int n;
    scanf("%d", &n);
    while(n < 1 || n > 4){
        printf("Please choose from 1 to 4: ");
        scanf("%d", &n);
    }
    system("clear");
    return n;
}

int seller_in_room_menu(){
    // system("clear");
    printf("---------------IN ROOM %d---------------\n", room_id);
    printf("1. View all auction item in this room\n");
    printf("2. Add your auction item into this room\n");
    printf("3. Leave room\n");
    // printf("4. Exit\n");
    printf("Please choose 1-3: ...\n");
    int n;
    scanf("%d", &n);
    while(n < 1 || n > 3){
        printf("Please choose from 1 to 3: ");
        scanf("%d", &n);
    }
    system("clear");
    return n;
}

char* create_item_menu(){
    char item[200];
    char name[30];
    memset(name, 0, sizeof(name));
    memset(item, 0, sizeof(item));
    int sample = 0;
    char x = '\0';
    int duration = 10000;
    int start_price = 10000;
    int buy_now_price = 100000;
    
    printf("---------------CREATE ITEM---------------\n");
    printf("Please provide some information to create item: \n");
    printf("Item name: ");
    scanf("%s", name);
    printf("Duration (mins): ");
    scanf("%d", &duration);
    printf("Start price (VND): ");
    scanf("%d", &start_price);
    printf("Buy now price (VND): ");
    scanf("%d", &buy_now_price);
    printf("Do you want to create item %s? (y/n)\n", name);
    char confirm;
    scanf("%s", &confirm);
    while(confirm != 'y' && confirm != 'n'){
        printf("Please choose y or n: ");
        scanf("%c", &confirm);
    }
    if (confirm == 'y'){
        sprintf(item, "%s , %d , %d , %d , %d , %s\0", name, start_price, buy_now_price, duration, room_id, username);
        char* resultCopy = strdup(item);
        printf("%s\n", resultCopy);
        return resultCopy;
    }else{
        return "0";
    }
}

void display_item(char* data){
    char *token;
    token = strtok(data, ";");
    char* name[20];
    char* owner[20];
    int start_price;
    int buy_now_price;
    int duration;
    int room_id;
    int id;
    int status;
    printf("---------------ITEM LIST---------------\n");
    printf("ID\tName\tOwner\tStart Price\tBuy Now Price\tDuration\tRoom\tStatus\n");
    while( token != NULL ) 
    {   
        sscanf(token, "%d , %s , %d , %d , %d , %d , %s , %d\0", &id, name, &start_price, &buy_now_price, &room_id, &duration, owner, &status);
        if (status == 0){
            printf("%d\t%.20s\t%.20s\t%d\t\t%d\t\t%d\t\t%d\t%.20s\n", id, name, owner, start_price, buy_now_price, duration, room_id, "New");
        }else if (status == 1){
            printf("%d\t%.20s\t%.20s\t%d\t\t%d\t\t%d\t\t%d\t%.20s\n", id, name, owner, start_price, buy_now_price, duration, room_id, "Waiting");
        }else if (status == 2){
            printf("%d\t%.20s\t%.20s\t%d\t\t%d\t\t%d\t\t%d\t%.20s\n", id, name, owner, start_price, buy_now_price, duration, room_id, "On Auction");
        }else if (status == 3){
            printf("%d\t%.20s\t%.20s\t%d\t\t%d\t\t%d\t\t%d\t%.20s\n", id, name, owner, start_price, buy_now_price, duration, room_id, "Sold");
        }
        token = strtok(NULL, ";");
    }
   return;
}
void display_schedule(char* data){
    //  1 , 2024-01-31 16:40:17 , 1970-01-01 07:09:59 , 1 , 1 , 0 ;
    char *token;
    token = strtok(data, ";");
    int id, item_id, room_id, status;
    char* start_time_date[30], start_time_hour[30];
    char* end_time_date[30], end_time_hour[30];
    printf("---------------SCHEDULE---------------\n");
    printf("ID\tStart Time\t\tEnd Time\t\tItem ID\tRoom ID\tStatus\n");
    while( token != NULL ) 
    {   
        // printf("%s\n", token);
        sscanf(token, "%d , %s %s , %s %s , %d , %d , %d\0", &id, start_time_date, start_time_hour, end_time_date, end_time_hour, &room_id, &item_id, &status);
        if (status == 0){
            printf("%d\t%s %s\t%s %s\t%d\t%d\t%s\n", id, start_time_date, start_time_hour, end_time_date, end_time_hour, item_id, room_id, "Waiting");
        }else if (status == 1){
            printf("%d\t%s %s\t%s %s\t%d\t%d\t%s\n", id, start_time_date, start_time_hour, end_time_date, end_time_hour, item_id, room_id, "Waiting");
        }else if (status == 2){
            printf("%d\t%s %s\t%s %s\t%d\t%d\t%s\n", id, start_time_date, start_time_hour, end_time_date, end_time_hour, item_id, room_id, "Waiting");
        }else if (status == 3){
            printf("%d\t%s %s\t%s %s\t%d\t%d\t%s\n", id, start_time_date, start_time_hour, end_time_date, end_time_hour, item_id, room_id, "Waiting");
        }
        token = strtok(NULL, ";");
    }
}