#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "./database/database_function.h"
#include "./server_function.h"
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */
#define START_HEADER 8
#define START_PAYLOAD 25

char username[20];
int room_id;


void process_message(char* msg, int n, int connfd);
void send_message(char* header, char* data, int connfd);

int main (int argc, char **argv)
{
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;
    
    //creation of the socket
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    perror("Problem in creating the socket");
    exit(2);
    }
            
    //preparation of the socket address 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    //bind the socket
    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    //listen to the socket by creating a connection queue, then wait for clients
    listen (listenfd, LISTENQ);
        
    printf("%s\n","Server running...waiting for connections.");

    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        //accept a connection
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
        
        printf("%s\n","Received request...");

        if ( (childpid = fork ()) == 0 ) { //if it’s 0, it’s child process
            
            // printf ("%s\n","Child created for dealing with client requests");
            
            //close listening socket
            close(listenfd);

            char hello_str[] = " ";
            send(connfd, hello_str, strlen(hello_str), 0);
            
            while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
                // printf("%s\n","String received from and resent to the client:");
                puts(buf);
                process_message(buf, n, connfd);
                memset(buf,0,strlen(buf));
                // char respone[MAXLINE] = process_message(buf, n);
                // send(connfd, respone, n, 0);
            }         
            if (n < 0) {
                perror("Read error"); 
            }
            exit(1);
        }
        //close server connection
        printf("user logout");
        close(connfd);
    }
}

void process_message(char* msg, int n, int connfd){
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

    if(strcmp(header, "LOGIN_REQ") == 0){
        int respone = login_request(data);
        if(respone == 0){
            send_message("LOGIN_RES", "0", connfd);
        }else if (respone == 1){
            send_message("LOGIN_RES", "1", connfd);
        }else if (respone == 2){
            send_message("LOGIN_RES", "2", connfd);
        }
        printf("\n");
        return;
    }
    if(strcmp(header, "REGISTER_") == 0){
        register_request(data);
        printf("\n");
        return;
    }
    if (strcmp(header, "VROOM_ALL\0") == 0){
        printf("VIEW_ROOM_ALL:\n");
        char* respone = search_all_auction_room_table_string(); //get all room 
        send_message("VROOM_RES", respone, connfd); //send all room to client
        free(respone); //free memory
        printf("\n");
        return;
    }
    if (strcmp(header, "CROOM_REQ") == 0){
        int respone = insert_rooms_table(data); //insert room to database
        if(respone == 0){
            send_message("CROOM_RES", "0", connfd); //send respone to client
        }else if (respone == 1){
            send_message("CROOM_RES", "1", connfd);
        }else if (respone == 2){
            send_message("CROOM_RES", "2", connfd);
        }
        printf("\n");
        return;
    }
    
}

void send_message(char* header, char* data, int connfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    sprintf(sendline, "HEADER: %s; DATA: %s", header, data);
    send(connfd, sendline, strlen(sendline), 0);
}