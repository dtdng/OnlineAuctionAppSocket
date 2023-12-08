CC = gcc
CFLAGS = -Wall

CLIENT_DIR = ./client
SERVER_DIR = ./server

IP_SERVER = 192.168.1.5


all: client.out server.out 


client.out: ./client/client.c
	${CC} ${CFLAGS} ${CLIENT_DIR}/client.c -o ${CLIENT_DIR}/client.out 

server.out: ./server/server.c
	$(CC) $(CFLAGS) ${SERVER_DIR}/server.c -o ${SERVER_DIR}/server.out

run_client: client.out
	${CLIENT_DIR}/client.out ${IP_SERVER}

run_server: server.out
	${SERVER_DIR}/server.out

clean: 
	rm -f ${SERVER_DIR}/*.out ${CLIENT_DIR}/*.out all