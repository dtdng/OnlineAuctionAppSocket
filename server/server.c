#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "./database/database_function.h"
#include "./server_function.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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
        
        printf("%s [SYSTEM] New client connected %s\n", ANSI_COLOR_MAGENTA, ANSI_COLOR_RESET);

        if ( (childpid = fork ()) == 0 ) { //if it’s 0, it’s child process
            // printf("username: %s\n", username);
            // printf ("%s\n","Child created for dealing with client requests");
            //close listening socket
            close(listenfd);

            char hello_str[] = " ";
            send(connfd, hello_str, strlen(hello_str), 0);
            
            while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
                // printf("%s\n","String received from and resent to the client:");
                printf("%s [SYSTEM] Received request: %s %s\n",ANSI_COLOR_GREEN, buf, ANSI_COLOR_RESET);
                // puts(buf);
                process_message(buf, n, connfd);
                memset(buf,0,strlen(buf));
                // char respone[MAXLINE] = process_message(buf, n);
                // send(connfd, respone, n, 0);
            }         
            if (n < 0) {
                perror("[SYSTEM] Read error"); 
            }
            logout_user(username);
            printf("%s [SYSTEM] User %s disconnected %s\n", ANSI_COLOR_YELLOW, username, ANSI_COLOR_RESET);
            memset(username,0,strlen(username));
            exit(1);
        }
        //close server connection
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
        int i = 10;
        while(data[i]!= ';'){
            username[i - 10] = data[i];
            i++;
        }
        printf("USERNAME: %s\n", username);

        int respone = login_request(data);
        if(respone == 0){
            send_message("LOGIN_RES", "0", connfd);
        }else if (respone == 1){
            send_message("LOGIN_RES", "1", connfd);
        }else if (respone == 2){
            send_message("LOGIN_RES", "2", connfd);
        }else if (respone == 3){
            send_message("LOGIN_RES", "3", connfd);
        }else if (respone == 4){
            send_message("LOGIN_RES", "4", connfd);
        }
        printf("\n");
        return;
    }
    if(strcmp(header, "REGIS_REQ") == 0){
        int respone = register_request(data);
        if(respone == 0){
            send_message("REGIS_RES", "0", connfd);
        }else if (respone == 1){
            send_message("REGIS_RES", "1", connfd);
        }else if (respone == 2){
            send_message("REGIS_RES", "2", connfd);
        }
        printf("\n");
        return;
    }
    if (strcmp(header, "VROOM_REQ\0") == 0){
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
    if (strcmp(header, "LOGOUT___") == 0){
        logout_user(data);
        printf("\n");
        return;
    }
    if (strcmp(header, "JROOM_REQ") == 0)
    {
        int respone = update_user_room_id(username, atoi(data));
        if(respone == 0){
            room_id = atoi(data);
            send_message("JROOM_RES", "0", connfd);
        }else if (respone == 1){
            send_message("JROOM_RES", "1", connfd);
        }else if (respone == 2){
            send_message("JROOM_RES", "1", connfd);
        }
        printf("\n");
    }
    if (strcmp(header, "CITEM_REQ") == 0)
    {
        char name[20], owner[20];
        int start_price, buyout_price, duration, room_id;
        sscanf(data, "%s , %d , %d , %d , %d , %s\0", name, &start_price, &buyout_price, &duration, &room_id, owner);
        int respone = insert_items_table(name, start_price, buyout_price, room_id , duration, owner);
        if(respone == 0){
            send_message("CITEM_RES", "0", connfd);
        }else if (respone == 1){
            send_message("CITEM_RES", "1", connfd);
        }else if (respone == 2){
            send_message("CITEM_RES", "2", connfd);
        }
        printf("\n");
    }
    if (strcmp(header, "MITEM_REQ") == 0)
    {
        char* respone = search_item_by_owner(username);
        send_message("MITEM_RES", respone, connfd);
        printf("Respone: %s\n", respone);
        free(respone);
        printf("\n");
    }
    if (strcmp(header, "BITEM_REQ") == 0)
    {
        int check = search_item_with_status_id(atoi(data), 0);
        printf("CHECK: %d\n", check);
        if (check == 1){
            send_message("BITEM_RES", "4", connfd);
        }
        else {
            int respone =  update_item(atoi(data), 1, room_id);
            if(respone == 0){
                send_message("BITEM_RES", "0", connfd);
            }else if (respone == 1){
                send_message("BITEM_RES", "1", connfd);
            }else if (respone == 2){
                send_message("BITEM_RES", "2", connfd);
            }
        }
        
        printf("\n");
    }
    if (strcmp(header, "VRITEMREQ") == 0)
    {
        char* respone = search_item_by_room_id(room_id);
        // printf("Respone: %s\n", respone);
        send_message("VRITEMRES", respone, connfd);
        free(respone);
        printf("\n");
    }
    if (strcmp(header, "VSCHE_REQ") == 0){
        char* respone = search_bids_session_by_room_id(room_id);
        send_message("VSCHE_RES", respone, connfd);
        free(respone);
        printf("\n");
    }
    printf("USERNAME: %s\n", username);
    printf("ROOM_ID: %d\n", room_id);
}

void send_message(char* header, char* data, int connfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    sprintf(sendline, "HEADER: %s; DATA: %s", header, data);
    printf("%s [SYSTEM] SEND: %s %s\n", ANSI_COLOR_CYAN, sendline, ANSI_COLOR_RESET);
    send(connfd, sendline, strlen(sendline), 0);
}