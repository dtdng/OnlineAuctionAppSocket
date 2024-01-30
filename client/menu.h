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
        sprintf(header, "REGISTER_\0");
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
    printf("1. Go Bidding\n");
    printf("2. Place auction item\n");
    printf("Please choose 1-2: ...\n");
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
    printf("2. Search auction item\n");
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
    printf("1. View all auction room\n");
    printf("2. Create auction item\n");
    printf("3. Join room\n");
    printf("4. Create room\n");
    printf("5. Back to choose role\n");
    printf("6. Exit\n");
    printf("Please choose 1-6: ...\n");
    int n; 
    scanf("%d", &n);
    while(n < 1 || n > 6){
        printf("Please choose from 1 to 6: ");
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
    system("clear");
    printf("---------------IN ROOM %d---------------\n", room_id);
    return 0;
}

int seller_in_room_menu(){
    system("clear");
    printf("---------------IN ROOM---------------\n", room_id);
    return 0;
}