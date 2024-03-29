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

char your_item[200];
int is_in_room = 0; //0: not in room, 1: in room
int room_id; 
char item_in_room[200];
void send_message(char* header, char* data, int sockfd);
// void send_message(char* msg, int sockfd);
void process_message(char* msg, int n);
int exit_function();

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

                if (is_in_room == 1) {
                    //TODO in room menu
                    int action = bidder_in_room_menu();
                    printf("ROOM_ID: %d\n", room_id);
                    if (action == 1){ // view all auction item in room
                        char header[] = "VRITEMREQ\0";
                        char data[5];
                        send_message(header, data, sockfd);
                    } else if (action == 2){ // view schedula
                        char header[] = "VSCHE_REQ\0";
                        char data[5];
                        send_message(header, data, sockfd);
                    } else if (action == 3){ // bid item

                    } else if (action == 4){ // back to bidder menu
                        is_in_room = 0;
                        system("clear");
                        continue;
                    }
                }else {
                    //TODO
                    int action = bidder_menu();
                    if (action == 1){ // view all auction room
                        char header[] = "VROOM_REQ\0";
                        char data[] = " ";
                        send_message(header, data, sockfd); 
                    }else if (action == 2){ // search auction item

                    }else if (action == 3){ // join room
                        printf("Please enter room id: ");
                        scanf("%d", &room_id);

                        char header[] = "JROOM_REQ\0";
                        char data[5];
                        sprintf(data, "%d\0", room_id);
                        
                        send_message(header, data, sockfd);  
                    }else if (action == 4){// back to choose role                        
                        role = choose_role();
                        continue;
                    }else if (action == 5){ //exit
                        int temp =  exit_function();
                        if (temp == 1){
                            exit(0);
                        }else if (temp == 0){
                            continue;
                        }
                    }
                }
                
            }else if (role == 2) { //seller
                if (is_in_room == 1) {
                    int action = seller_in_room_menu();
                    //TODO in room menu
                    if (action == 1){ // view all auction item in room
                        
                        char header[] = "VRITEMREQ\0";
                        char data[5];
                        send_message(header, data, sockfd);
                    } else if (action == 2){ // place an item to bid in the room
                        printf("Your item: \n");
                        display_item(your_item);
                        printf("Choose an item to place a bid: ");
                        int item_id;
                        scanf("%d", &item_id);
                        char header2[] = "BITEM_REQ\0";
                        char data2[5];
                        sprintf(data2, "%d\0", item_id);

                        send_message(header2, data2, sockfd);
                    } else if (action == 3){ // back to bidder menu
                        is_in_room = 0;

                        //TODO add request leave room
                        system("clear");
                        continue;
                    } else if (action == 4){
                        /* code */
                    }
                }else { //not in room
                    //TODO
                    int action = seller_menu();
                    if (action == 3){ // view all auction room
                        char header[] = "VROOM_REQ\0";
                        char data[] = " ";
                        send_message(header, data, sockfd); 
                    }else if (action == 1){ // create auction item
                        char header[] = "CITEM_REQ\0";
                        char* data = create_item_menu();
                        send_message(header, data, sockfd);
                        system("clear");
                        action = 0;
                    }else if (action == 4){ //join room

                        printf("Please enter room id: ");
                        scanf("%d", &room_id);
                        char header[] = "JROOM_REQ\0";
                        char data[5];
                        sprintf(data, "%d\0", room_id);
                        send_message(header, data, sockfd);  
                    }else if (action == 5){ // create room
                        char* room_name = create_room();
                        if(strcmp(room_name, "0") != 0){
                            send_message("CROOM_REQ", room_name, sockfd);
                        }
                    }else if (action == 6){ // back to choose role
                        role = choose_role();
                        continue;
                    }else if (action == 7)
                    {
                        int temp =  exit_function();
                        if (temp == 1){
                            exit(0);
                        }else if (temp == 0){
                            continue;
                        }
                    }else if (action == 2){ //view all item of seller
                        char header[] = "MITEM_REQ\0"; 
                        char data[5];
                        sprintf(data, "%d\0", room_id);
                        send_message(header, data, sockfd);
                    }
                }
            }
        }              
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
    char header[10], x = '\0', data[MAXLINE]; 
    memset(header,0,strlen(header));
    memset(data,0,strlen(data));
    for(int i = 0; i < START_HEADER+1; i++){
        header[i] = msg[START_HEADER+i];
    }
    
    for(int i = 0; i < n-START_PAYLOAD; i++){
        data[i] = msg[START_PAYLOAD+i];
    }
    data[n-START_PAYLOAD] = '\0';
    printf("header received: %s\n" ,header);
    printf("payload received: %s\n" ,data);
    
    // process message with each header
    if (strcmp(header, "LOGIN_RES") == 0){
        if(strcmp(data, "0") == 0) {
            LOG_GREEN("Login successed");
            logged = true;
        }
        else if (strcmp(data, "1") == 0 || strcmp(data, "2") == 0)  {
            LOG_RED("Wrong password or username!!");
            logged = false;
        }
        else if (strcmp(data, "4") == 0)  {
            LOG_RED("Account is already logged in!!");
            logged = false;
        } else {
            LOG_RED("Database has error!!");
            logged = false;
        }
        printf("\n");
        return;
    }else if (strcmp(header, "REGIS_RES") == 0){
        // register_request(data);
        if(strcmp(data, "0") == 0) {
            LOG_GREEN("Create account successed");
        }
        else if (strcmp(data, "1") == 0)  {
            LOG_RED("Account has already existed!!");
        }else {
            LOG_RED("Database has error!!");
        }
        printf("\n");
        return;
    }else if (strcmp(header, "VROOM_RES") == 0){
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
    }else if (strcmp(header, "CROOM_RES") == 0){
        if (strcmp(data, "0") == 0){
            LOG_GREEN("Room created successfully\n");
        }else if (strcmp(data, "1") == 0){
            LOG_RED("Room name already exist\n");
        }else if (strcmp(data, "2") == 0){
            LOG_RED("Database has error\n");
        }
    }else if (strcmp(header, "JROOM_RES") == 0){
        if (strcmp(data, "0") == 0){
            is_in_room = 1;
            LOG_GREEN("Join room successfully\n");
        }else if (strcmp(data, "1") == 0){
            LOG_RED("Database has error\n");
        }else if (strcmp(data, "2") == 0){
            LOG_RED("Room not exist\n");
        }
    }else if (strcmp(header, "CITEM_RES") == 0)
    {
        if (strcmp(data, "0") == 0){
            LOG_GREEN("Create item successfully\n");
        }else if (strcmp(data, "1") == 0){
            LOG_RED("Database has error\n");
        }else if (strcmp(data, "2") == 0){
            LOG_RED("Create item fail\n");
        }
    }else if (strcmp(header, "MITEM_RES") == 0)
    {
        if (strcmp(data, "You have no item") == 0){
            LOG_RED("You have no item\n");
        }else if (strcmp(data, "Error") == 0){
            LOG_RED("Error\n");
        }else
        {
            strcpy(your_item, data);
            printf("Your item: \n");
            display_item(data);
        }
    }else if (strcmp(header, "VRITEMRES") == 0)
    {
        if (strcmp(data, "This room have no item on queue") == 0){
            LOG_RED("This room have no item on queue\n");
        }else if (strcmp(data, "Error") == 0){
            LOG_RED("Error\n");
        }else
        {
            printf("Item in this room: \n");
            strcpy(item_in_room, data);
            display_item(item_in_room);
        }
    }else if (strcmp(header, "VSCHE_RES") == 0){
        if (strcmp(data, "This room have no item on queue") == 0){
            LOG_RED("This room have no item on queue\n");
        }else if (strcmp(data, "Error") == 0){
            LOG_RED("Error\n");
        }else
        {
            printf("Schedule in this room: \n");
            display_schedule(data);
        }
    }
    else if (strcmp(header, "BITEM_RES") == 0){
        if (strcmp(data, "0") == 0){
            LOG_GREEN("Item is place in room successfully\n");
        }else if (strcmp(data, "1") == 0){
            LOG_RED("Database has error\n");
        }else if (strcmp(data, "2") == 0){
            LOG_RED("Place item fail\n");
        }else if (strcmp(data, "3") == 0){
            LOG_RED("Item is not belong to you\n");
        }else if (strcmp(data, "4") == 0){
            LOG_RED("Item is already on queue\n");
        }
    }
    memset(header,0,strlen(header));
    memset(data,0,strlen(data));
}

void send_message(char* header, char* data, int sockfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    sprintf(sendline, "HEADER: %s; DATA: %s", header, data);
    send(sockfd, sendline, strlen(sendline), 0);
};

int exit_function(){
    printf("Do you want to exit\n");
    printf("1. Yes\n");
    printf("2. No\n");
    int n;
    scanf("%d", &n);
    while(n != 1 && n != 2){
        printf("Please choose 1 - 2: ");
        scanf("%d", &n);
    }
    if (n == 1){
        return 1;   //exit
    }
    system("clear");
    return 0; //not exit
}