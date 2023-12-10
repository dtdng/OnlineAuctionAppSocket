#include <iostream> 
#include <sqlite3.h> 
#include <string.h>
#include "database_function.h"

using namespace std;

int main(int argc, char** argv) 
{ 
	create_account_table();
	insert_account_table("dtdng", "12345678");
	insert_account_table("acc1", "12345678");
	insert_account_table("acc2", "12345678");
	insert_account_table("acc2", "123456789");
	insert_account_table("acc3", "12345678");
	search_account_table("acc5");
	cout << "Check login acc3 " << check_login("acc10", "1234678") << endl;
	return (0); 
} 
