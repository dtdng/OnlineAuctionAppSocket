#pragma once

#include <stdio.h> 
#include <string.h> 
#include <syscall.h>

typedef struct Account{
    char username[20];
    char password[20];
} Account;

char username[20];

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
        printf("Please choose 1 or 2: ");
        scanf("%d", &n);
    }
    return n;
}