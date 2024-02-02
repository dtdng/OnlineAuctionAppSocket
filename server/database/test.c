#include <stdio.h> 
#include <sqlite3.h> 
#include <string.h>
#include "database_function.h"
#define DATABASE_FILE "database.db"
// using namespace std;

int main(int argc, char** argv) 
{ 
	create_account_table();
	create_auction_room_table();
	create_items_table();
	create_bids_table();
	create_bids_session_table();
	insert_account_table("dtdng", "12345678");
	insert_account_table("acc1", "12345678");
	insert_account_table("acc2", "12345678");
	insert_account_table("acc2", "123456789");
	insert_account_table("acc3", "12345678");
	// search_account_table("acc5");
	printf("%d", check_login("dtdng", "12345678"));
	printf("%d", check_login("dtdng", "123456789"));
	printf("%d", check_login("dtdng", "12345678"));
	update_user_room_id("dtdng", 1);
	
	logout_user("dtdng");
	insert_rooms_table("A's auction");
	insert_rooms_table("B's auction");
	insert_rooms_table("B's auction");
	insert_rooms_table("C's auction");

	
	// search_auction_room_by_id(1);
	search_auction_room_by_name("C's auction");
	search_all_auction_room_table();
	search_all_auction_room_table_string();
	insert_items_table("item1", 10, 1000, 1, 12, "acc1");
	insert_items_table("item2", 200, 2000, 1, 1, "acc2");
	insert_items_table("item3", 300, 3000, 1, 2, "acc2");
	insert_items_table("item4", 400, 4000, 2, 3, "acc3");

	insert_bids_session_table(1, 1, 10);
	insert_bids_session_table(1, 2, 10);
	insert_bids_session_table(1, 3, 20);
	insert_bids_session_table(2, 4, 50);
	// search_item_by_name("item1");
	// search_item_by_owner("Seller2");
 	// char* respone = search_item_by_owner("Seller2");
	// printf("%s\n", respone);
	// char* respone2 =search_item_by_room_id(1);	
	// printf("%s\n", respone2);
	// char* respone3 =search_item_by_room_id(4);	
	// printf("%s\n", respone3);


	char* respone = search_bids_session_by_room_id(1);
	printf("%s\n", respone);
	// char* respone2 = search_last_bids_session_by_room_id(1);
	// printf("%s\n", respone2);


	char* bid_start_time = get_current_time();
	printf("%s\n", bid_start_time);
	int duration = 10000;
    char* bid_end_time = get_end_time(bid_start_time ,duration);
	printf("%s\n", bid_end_time);

	// char* respone5 = search_last_bids_session_by_room_id(1);
	// printf("%s\n", respone5);

	// insert_bids_session_table(1, 1, 10);
	// insert_bids_session_table(1, 2, 10);
	// char* respone4 = search_last_bids_session_by_room_id(1);
	// printf("%s\n", respone4);
	return (0); 
} 
