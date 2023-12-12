#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "menu.h"
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/

void sendMessage(char* header, char* data, int sockfd);

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
    printf("Header: %s\n", header);
    printf("Data: %s\n", data);
    sendMessage(header, data, sockfd);
    
    
    // while (fgets(sendline, MAXLINE, stdin) != NULL) {
    //     memset(recvline, '\0', sizeof(recvline)); 
    //     send(sockfd, sendline, strlen(sendline), 0);
                
    //     if (recv(sockfd, recvline, MAXLINE,0) == 0){
    //         //error: server terminated prematurely
    //         perror("The server terminated prematurely"); 
    //         exit(4);
    //     }
    //     printf("%s", "String received from the server: ");
    //     fputs(recvline, stdout);
    // }

    exit(0);
}

void sendMessage(char* header, char* data, int sockfd){
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