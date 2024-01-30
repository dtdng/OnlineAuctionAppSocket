CC = gcc
# CFLAGS = 
CFLAGS = -w
DBFLAGS = -l sqlite3

CLIENT_DIR = ./client
SERVER_DIR = ./server
TESTDB_DIR = ./server/database

IP_SERVER = 192.168.56.1


all: client.out server.out 


client.out: ./client/client.c
	${CC} ${CFLAGS} ${CLIENT_DIR}/client.c -o ${CLIENT_DIR}/client.out 

server.out: ./server/server.c
	$(CC) $(CFLAGS) ${SERVER_DIR}/server.c -o ${SERVER_DIR}/server.out ${DBFLAGS}

test.out: 
	${CC} ${CFLAGS} ${TESTDB_DIR}/test.c -o ${TESTDB_DIR}/test.out ${DBFLAGS}

run_client: client.out
	${CLIENT_DIR}/client.out ${IP_SERVER} 

run_server: server.out
	${SERVER_DIR}/server.out

# run_testdb: clean_db
# 	./${TESTDB_DIR}/test.out

# clean_db: 
# 	rm -f ${TESTDB_DIR}/*.db

clean: 
	rm -f ${SERVER_DIR}/*.out ${CLIENT_DIR}/*.out all