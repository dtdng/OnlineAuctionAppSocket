#pragma once
#include <stdio.h> 
#include <sqlite3.h> 
#include <string.h> 
// #define DATABASE_FILE "database.db"
#define DATABASE_FILE "./server/database/database.db"

sqlite3* db;

typedef struct Account{
    char username[20];
    char password[20];
} Account;

typedef struct rooms
{
    int room_id;
    char room_name[20];
} Room;

void open_database() {
    if (sqlite3_open(DATABASE_FILE, &db) != SQLITE_OK) {
        
        fprintf(stderr, "Error: Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}

void close_database() {
    sqlite3_close(db);
}

static int callback(void* data, int argc, char** argv, char** azColName) 
{ 
    int i; 
    fprintf(stderr, "%s: ", (const char*)data); 
  
    for (i = 0; i < argc; i++) { 
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL"); 
    } 
  
    printf("\n"); 
    return 0; 
} 

void create_account_table(){
    open_database();
	const char* create_table_query =
        "CREATE TABLE IF NOT EXISTS accounts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL,"
        "status INTEGER DEFAULT 0," // 0 = offline, 1 = online, 2 = in room, 3 = in auction, 4 = in bidding
        "room_id INTEGER DEFAULT 0" // 0 = not in room
        ");";

    if (sqlite3_exec(db, create_table_query, 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Error: Can't create table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
}

int search_account_table(char* username){
    open_database();
    int check = 0; 
    const char* select_query = "SELECT * FROM accounts WHERE username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            const char* retrievedUsername = (const char*)sqlite3_column_text(stmt, 1);
            const char* retrievedPassword = (const char*)sqlite3_column_text(stmt, 2);

            printf("Account found:\n");
            printf("ID: %d\n", id);
            printf("Username: %s\n", retrievedUsername);
            printf("Password: %s\n", retrievedPassword);

        }

        sqlite3_finalize(stmt);
        if(check == 1) return 0;
        return 2;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    close_database();
    // return state
    // 0 = found
    // 1 = fail
    // 2 = not found
}

int insert_account_table(char* username, char* password){

    open_database();
    int check_username_exist = search_account_table(username);

    if(check_username_exist == 0){
        fprintf(stderr, "Username has existed Cannot Add\n");
        return 1;
    }
    const char* insert_query = "INSERT INTO accounts (username, password) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, insert_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Error: Can't insert data: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Account inserted successfully.\n");
        }

        sqlite3_finalize(stmt);
        return 0;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return 2;
    }
    close_database();
    // return state
    // 0 = success
    // 1 = fail, account exists
    // 2 = fail
}

int check_login(char* username, char* password){
    open_database();
    int onlineStatus = -1 ;
    int check = -1; 
    const char* select_query = "SELECT * FROM accounts WHERE username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            const char* retrievedPassword = (const char*)sqlite3_column_text(stmt, 2);
            onlineStatus = sqlite3_column_int(stmt, 3); 
            check = strcmp(password, retrievedPassword);
            
        }
        
        sqlite3_finalize(stmt);
        
        if(check == 0) {
            if (onlineStatus == 1) {
                fprintf(stderr, "Error: User already logged in.\n");
                close_database();
                return 4; // User already logged in
            } else {
                // Update online status to indicate that the user is now online
                update_user_online_status(username, 1);
                printf("Login successful. User is now online.\n");
                close_database();
                return 0; // Success
            }
        };
        if(check == 1) return 1; 
        return 2;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        close_database();
        return 3;
    }

    close_database();

    // return state
    // 0 = found & correct pwd
    // 1 = found & wrong pwd
    // 2 = not found
    // 3 = err
}

void update_user_online_status(char* username, int online_status) {
    open_database();

    const char* update_query = "UPDATE accounts SET status = ? WHERE username = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, update_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, online_status);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("User %s online status updated successfully.\n", username);
        } else {
            fprintf(stderr, "Error: Can't update online status: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
    }

    close_database();
}

void logout_user(char* username) {
    update_user_room_id(username, 0);
    update_user_online_status(username, 0);
}

void create_auction_room_table(){
    open_database();
	const char* create_table_query =
        "CREATE TABLE IF NOT EXISTS rooms ("
        "room_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "room_name TEXT NOT NULL"
        ");";

    if (sqlite3_exec(db, create_table_query, 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Error: Can't create table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
}

void create_items_table(){
    open_database();
	const char* create_table_query =
        "CREATE TABLE IF NOT EXISTS items ("
            "item_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "item_name TEXT NOT NULL,"
            "starting_price REAL NOT NULL,"
            "buy_now_price REAL,"
            "room_id INTEGER NOT NULL,"
            "duration INTEGER NOT NULL,"
            "owner TEXT NOT NULL,"
            "status INTEGER DEFAULT 0," // 0 = waiting, 1 = on bidding, 2 = sold
            "FOREIGN KEY (room_id) REFERENCES rooms(room_id)"
        ");";

    if (sqlite3_exec(db, create_table_query, 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Error: Can't create table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
}

void create_bids_table(){
    open_database();
	const char* create_table_query =
        "CREATE TABLE bids ("
        "bid_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "amount DECIMAL(10, 2) NOT NULL,"
        "bid_time DATETIME NOT NULL,"
        "user_id INTEGER,"
        "item_id INTEGER,"
        "FOREIGN KEY (user_id) REFERENCES accounts(user_id),"
        "FOREIGN KEY (item_id) REFERENCES items(item_id));";

    if (sqlite3_exec(db, create_table_query, 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Error: Can't create table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
}

int search_all_auction_room_table(){
    open_database();
    int check = 0; 
    const char* select_query = "SELECT * FROM rooms;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        // sqlite3_bind_text(stmt, 1, room_id, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            // const char* retrievedRoomID = (const char*)sqlite3_column_text(stmt, 1);
            const char* retrievedRoomName = (const char*)sqlite3_column_text(stmt, 1);

            printf("Room found:\n");
            printf("Room ID: %d\n", id);
            printf("Room name: %s\n", retrievedRoomName);

        }

        sqlite3_finalize(stmt);
        if(check == 1) return 0;
        return 2;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    close_database();
    // return state
    // 0 = found
    // 1 = fail
    // 2 = not found
}

char* search_all_auction_room_table_string(){
    char result[1000];
    result[0] = '\0';
    open_database();
    int check = 0; 
    const char* select_query = "SELECT * FROM rooms;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        // sqlite3_bind_text(stmt, 1, room_id, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            // const char* retrievedRoomID = (const char*)sqlite3_column_text(stmt, 1);
            const char* retrievedRoomName = (const char*)sqlite3_column_text(stmt, 1);
            printf("Room ID: %d\n", id);
            printf("Room name: %s\n", retrievedRoomName);
            char temp[100];
            sprintf(temp, "%d, %s;", id, retrievedRoomName);
            strcat(result, temp);
        }
        sqlite3_finalize(stmt);
        char* resultCopy = strdup(result);
        if(check == 1) return resultCopy;
        return "Not found";
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return "Error";
    }
    close_database();
    // return state
    // 0 = found
    // 1 = fail
    // 2 = not found
}

int search_auction_room_by_id(int room_id){
    open_database();
    int check = 0; 
    const char* select_query = "SELECT * FROM rooms WHERE room_id = ? ;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, room_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            // const char* retrievedRoomID = (const char*)sqlite3_column_text(stmt, 0);
            // const char* retrievedRoomName = (const char*)sqlite3_column_text(stmt, 1);
            // printf("Room found:\n");
            // printf("Room ID: %d\n", id);
            // printf("Room name: %s\n", retrievedRoomName);

        }

        sqlite3_finalize(stmt);
        if(check == 1) return 0;
        return 2; 
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    close_database();
}

int search_auction_room_by_name_i(char* room_name){
    open_database();
    int check = 0; 

    const char* select_query = "SELECT * FROM rooms WHERE room_name = ? ;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, room_name, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            const char* retrievedRoomName = (const char*)sqlite3_column_text(stmt, 1);
        }

        sqlite3_finalize(stmt);
        if(check == 1) return 0;
        return 1;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
    }
    close_database();
    return 2;
    // return state
    // 0 = found
    // 1 = fail
    // 2 = not found
}

Room search_auction_room_by_name(char* room_name){
    open_database();
    int check = 0; 
    Room result;

    const char* select_query = "SELECT * FROM rooms WHERE room_name = ? ;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, room_name, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            // const char* retrievedRoomID = (const char*)sqlite3_column_text(stmt, 0);
            const char* retrievedRoomName = (const char*)sqlite3_column_text(stmt, 1);

            printf("Room found:\n");
            printf("Room ID: %d\n", id);
            printf("Room name: %s\n", retrievedRoomName);
            
            result.room_id = id;
            strcpy(result.room_name, retrievedRoomName);
        }

        sqlite3_finalize(stmt);
        // if(check == 1) return 0;
        // return result;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
    }
    close_database();
    // return result;
    return result;
    // return state
    // 0 = found
    // 1 = fail
    // 2 = not found
    
}

int insert_rooms_table(char* room_name){
    open_database();
    int check_room_exist = search_auction_room_by_name_i(room_name);
    if(check_room_exist == 0){
        fprintf(stderr, "Room %s has existed cannot add\n", room_name);
        return 1;
    }
    const char* insert_query = "INSERT INTO rooms (room_name) VALUES (?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, insert_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, room_name, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Error: Can't insert data: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Rooms %s inserted successfully.\n", room_name);
        }

        sqlite3_finalize(stmt);
        return 0;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return 2;
    }
    close_database();
    // return state
    // 0 = success
    // 1 = fail, rooms exists
    // 2 = fail
}

int search_all_item_table(){
    open_database();
    int check = 0; 
    const char* select_query = "SELECT * FROM items;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        // sqlite3_bind_text(stmt, 1, room_id, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
        }
        sqlite3_finalize(stmt);
        if(check == 1) return 0;
        return 2;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    close_database();
    // return state
    // 0 = found
    // 1 = fail
    // 2 = not found
}

int insert_items_table(char* item_name, int starting_price, int buy_now_price,int room_id, int duration, char* owner){
    open_database();
    
    const char* insert_query = "INSERT INTO items (item_name, starting_price, buy_now_price, room_id, duration, owner) VALUES (?, ?, ?, ?, ?, ?);"; 
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, insert_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item_name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, starting_price);
        sqlite3_bind_int(stmt, 3, buy_now_price);
        sqlite3_bind_int(stmt, 4, room_id);
        sqlite3_bind_int(stmt, 5, duration);
        sqlite3_bind_text(stmt, 6, owner, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Error: Can't insert data: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Items %s inserted successfully.\n", item_name);
        }

        sqlite3_finalize(stmt);
        close_database();
        return 0;
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        close_database();
        return 2;
    }
    // return state
    // 0 = success
    // 1 = fail, account exists
    // 2 = fail
}

char* search_item_by_name(char* item_name){
    char result[1000];
    result[0] = '\0';

    open_database();
    int check = 0; 
    const char* select_query = "SELECT * FROM items WHERE item_name = ? ;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item_name, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            char* item_name = sqlite3_column_text(stmt, 1);
            int starting_price = sqlite3_column_int(stmt, 2);
            int buy_now_price = sqlite3_column_int(stmt, 3);
            int room_id = sqlite3_column_int(stmt, 4);
            int duration = sqlite3_column_int(stmt, 5);
            char* owner = sqlite3_column_text(stmt, 6);

            char temp[100];
            sprintf(temp, "%d, %s, %d, %d, %d, %d, %s;", id, item_name, starting_price, buy_now_price, room_id, duration, owner);
            strcat(result, temp);
        }
        printf("%s\n", result);
        sqlite3_finalize(stmt);
        if(check == 1) return result;
        return "Not found";
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return "Error";
    }

    close_database();
}   

char* search_item_by_room_id(int room_id){
    char result[1000];
    result[0] = '\0';

    open_database();
    int check = 0; 
    const char* select_query = "SELECT * FROM items WHERE room_id = ? ;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, room_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            check = 1; 
            int id = sqlite3_column_int(stmt, 0);
            char* item_name = sqlite3_column_text(stmt, 1);
            int starting_price = sqlite3_column_int(stmt, 2);
            int buy_now_price = sqlite3_column_int(stmt, 3);
            int room_id = sqlite3_column_int(stmt, 4);
            int duration = sqlite3_column_int(stmt, 5);
            char* owner = sqlite3_column_text(stmt, 6);

            char temp[100];
            sprintf(temp, "%d, %s, %d, %d, %d, %d, %s;", id, item_name, starting_price, buy_now_price, room_id, duration, owner);
            strcat(result, temp);
        }
        printf("%s\n", result);
        sqlite3_finalize(stmt);
        if(check == 1) return result;
        return "Not found";
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        return "Error";
    }
    close_database();
}

int update_user_room_id(char* username, int room_id) {
    open_database();

    const char* update_query = "UPDATE accounts SET room_id = ? WHERE username = ?;";
    sqlite3_stmt* stmt;

    
    if(room_id != 0) {
        int check = search_auction_room_by_id(room_id);
        if(check == 2){
           fprintf(stderr, "Room %d not found\n", room_id);
            return 2;
        }
    }

    if (sqlite3_prepare_v2(db, update_query, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, room_id);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            if(room_id == 0)
                printf("User %s has left room\n", username);
            else
                printf("User %s has joined room %d\n", username, room_id);
        } else {
            fprintf(stderr, "Error: Can't update room id: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
        close_database();
        return 0; // Success
    } else {
        fprintf(stderr, "Error: Can't prepare statement: %s\n", sqlite3_errmsg(db));
        close_database();
        return 1; // Error
    }
}