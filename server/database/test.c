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
	insert_account_table("dtdng", "12345678");
	insert_account_table("acc1", "12345678");
	insert_account_table("acc2", "12345678");
	insert_account_table("acc2", "123456789");
	insert_account_table("acc3", "12345678");
	// search_account_table("acc5");
	// printf("%d", check_login("dtdng", "12345678"));

	insert_rooms_table("A's auction");
	insert_rooms_table("B's auction");
	insert_rooms_table("B's auction");
	insert_rooms_table("C's auction");
	// search_auction_room_by_id("1");
	// search_auction_room_by_name("C's auction");
	// search_all_auction_room_table();
	// search_all_auction_room_table_string();
	insert_items_table("item1", 10, 1000, 1, 12, "Seller");
	insert_items_table("item2", 200, 2000, 1, 1, "Seller2");
	insert_items_table("item3", 300, 3000, 1, 2, "Seller2");
	return (0); 
} 
