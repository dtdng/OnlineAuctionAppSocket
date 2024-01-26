#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include "menu.h"
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define START_HEADER 8
#define START_PAYLOAD 25

// store status
char username[20];
bool logged = false;
int role = 0; //1: bidder, 2: seller, 0: not choose
int is_in_room = 0; //0: not in room, 1: in room
int room_id; 

void send_message(char* header, char* data, int sockfd);
// void send_message(char* msg, int sockfd);
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
    system("clear");
    fputs(recvline, stdout);
    memset(recvline,0,strlen(recvline));

    // while (fgets(sendline, MAXLINE, stdin) != NULL) {
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        memset(recvline, '\0', sizeof(recvline)); 
        if (logged == false){
            char* msg = login_register_function();
            send(sockfd, msg, strlen(msg), 0);
        }else {
            if (role == 0){
                role = choose_role();
            }
            if (role == 1) { //bidder
                int action = bidder_menu();
                if (action == 1){
                    // view all auction room
                    char header[] = "VROOM_ALL\0";
                    char data[] = " ";
                    send_message(header, data, sockfd); 
                }else if (action == 2){
                    // search auction item
                    // char header[] = "SEARCH_ITEM\0";
                    // char data[] = " ";
                    // send_message(header, data, sockfd);
                } //search auction item
                else if (action == 3){
                    // join room
                    int room_id;
                    printf("Please enter room id: ");
                    scanf("%d", &room_id);

                    char header[] = "JOIN_ROOM\0";
                    char data[5];
                    sprintf(data, "%d\0", room_id);
                    
                    send_message(header, data, sockfd);  
                } else if (action == 4){
                    // back to choose role
                    role = choose_role();
                    continue;
                }
            }else if (role == 2) { //seller
                int action = seller_menu();
                if (action == 1){
                    // view all auction room
                    char header[] = "VROOM_ALL\0";
                    char data[] = " ";
                    send_message(header, data, sockfd); 
                }else if (action == 2){
                    //TODO
                }else  if (action == 3){
                    //TODO
                }else if (action == 4){
                    //TODO
                    char* room_name = create_room();\
                    
                    if(strcmp(room_name, "0") == 0){
                        continue;
                    }
                    send_message("CROOM_REQ", room_name, sockfd);
                }else if (action == 5){
                    // back to choose role
                    role = choose_role();
                    continue;
                }
                
            }
        }
        
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
    
    // process message with each header
    if (strcmp(header, "LOGIN_RES") == 0){
        if(strcmp(data, "0") == 0) {
            LOG_GREEN("Login successed");
            logged = true;
        }
        else {
            LOG_RED("Wrong password or username!!");
            logged = false;
        }
        printf("\n");
        return;
    }else if (strcmp(header, "REGIS_RES") == 0){
        // register_request(data);
        printf("\n");
        return;
    }
    else if (strcmp(header, "VROOM_RES") == 0){
        char *token,*nameRoom;
        int idRoom;
        token = strtok(data, ";");
        printf("VIEW_ROOM_ALL:\n");
        // TODO process data to get id and room_name 
        if(token == NULL){
            printf("No room available\n");
            return;
        }
        while( token != NULL ) 
        {
            printf( " %s\n", token ); //printing each token
            token = strtok(NULL, ";");
        }
        printf("\n");
        free(token);
        return;
    } else if (strcmp(header, "CROOM_RES") == 0){
        if (strcmp(data, "0") == 0){
            LOG_GREEN("Room created successfully\n");
        }else if (strcmp(data, "1") == 0){
            LOG_RED("Room name already exist\n");
        }else if (strcmp(data, "2") == 0){
            LOG_RED("Database has error\n");
        }
        
    }
}

void send_message(char* header, char* data, int sockfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    sprintf(sendline, "HEADER: %s; DATA: %s", header, data);
    send(sockfd, sendline, strlen(sendline), 0);
};