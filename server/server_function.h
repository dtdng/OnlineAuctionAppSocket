#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "./database/database_function.h"

void login_request(char* data){
    printf("Payload: %s\n", data);
    char username[20];
    int i = 10;
    char password[20];
    memset(username,0,strlen(username));
    memset(password,0,strlen(password));
    while(data[i]!= ';'){
        username[i - 10] = data[i];
        i++;
    }
    i += 12;
    int start_pwd_part = i;
    while(i < strlen(data)){
        password[i - start_pwd_part] = data[i];
        i++;
    }
    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    int status = check_login(username,password);
    if(status == 0){
        printf("Login successfully!!!!");
    }else if(status == 1){
        printf("Your information is not correct!");
    }else if(status == 2){
        printf("Your information is not correct!");
    }
    return;
}