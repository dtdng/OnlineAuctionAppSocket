#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "./database/database_function.h"

int login_request(char* data){
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
    int status = check_login(username,password);

    return status;
}

int register_request(char* data) {
    char username[20];
    int i = 10;
    char password[20];
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));

    // Extract username
    while (data[i] != ';') {
        username[i - 10] = data[i];
        i++;
    }

    i += 12; // Move past the ";password:" part
    int start_pwd_part = i;

    // Extract password
    while (i < strlen(data)) {
        password[i - start_pwd_part] = data[i];
        i++;
    }
    // Perform registration check
    int status = insert_account_table(username, password);

    return status;
}