#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "menu.h"
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define START_HEADER 8
#define START_PAYLOAD 25

void send_message(char* header, char* data, int sockfd);
void process_message(char* msg, int n);

int main(int argc, char **argv) 
{
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    //basic check of the arguments
    //additional checks can be inserted
    if (argc !=2) {
        perror("Usage: TCPClient <IP address of the server"); 
        exit(1);
    }
        
    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        perror("Problem in creating the socket");
        exit(2);
    }
        
    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
        
    //Connection of the client to the socket 
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        perror("Problem in connecting to the server");
        exit(3);
    }
    if (recv(sockfd, recvline, MAXLINE,0) == 0){
        //error: server terminated prematurely
        perror("The server terminated prematurely"); 
        exit(4);
    }
    printf("\n");
    fputs(recvline, stdout);
    memset(recvline,0,strlen(recvline));
    // 
    int login_or_register = display_welcome_menu();
    Account sender_acc;
    char header[12];
    char data[100];
    if (login_or_register == 1){
        sender_acc = display_login_menu();
        sprintf(header, "LOGIN_REQ\0");
        sprintf(data, "username: %s; password: %s\0", sender_acc.username, sender_acc.password);
    }else {
        sender_acc = display_register_menu();
        sprintf(header, "REGISTER_\0");
        sprintf(data, "username: %s; password: %s\0", sender_acc.username, sender_acc.password);
    }
    send_message(header, data, sockfd);
    //
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        memset(recvline, '\0', sizeof(recvline)); 
        // send(sockfd, sendline, strlen(sendline), 0);
                
        if (recv(sockfd, recvline, MAXLINE,0) == 0){
            //error: server terminated prematurely
            perror("The server terminated prematurely"); 
            exit(4);
        }
        process_message(recvline, strlen(recvline));
    }
    exit(0);
}

void process_message(char* msg, int n){
    char header[10], data[MAXLINE]; 
    memset(header,0,strlen(header));
    memset(data,0,strlen(data));
    for(int i = 0; i < START_HEADER+1; i++){
        header[i] = msg[START_HEADER+i];
    }
    
    for(int i = 0; i < n-START_PAYLOAD; i++){
        data[i] = msg[START_PAYLOAD+i];
    }
    printf("header received: %s\n" ,header);
    printf("payload received: %s\n" ,data);
    if(strcmp(header, "LOGIN_RES") == 0){
        if(strcmp(data, "0") == 0) printf("login successed");
        else printf("wrong pwd or username");
        printf("\n");
        return;
    }

    if(strcmp(header, "REGIS_RES") == 0){
        // register_request(data);
        printf("\n");
        return;
    }
}

void send_message(char* header, char* data, int sockfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    sprintf(sendline, "HEADER: %s; DATA: %s", header, data);
    send(sockfd, sendline, strlen(sendline), 0);

    // if (recv(sockfd, recvline, MAXLINE,0) == 0){
    //     //error: server terminated prematurely
    //     perror("The server terminated prematurely"); 
    //     exit(4);
    // }
    // printf("%s", "Respone from the server: ");
    // fputs(recvline, stdout);
}