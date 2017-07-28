#include "B_Plus_Tree.h"
#include "database.h"
#include <iostream>

using namespace std;

int open_db();
int create_db();
int info();
int handle_db(DB& db);

int main()
{
	cout << "/*   SQLight    */" << endl;
	cout << "Welcome!" << endl;

	/* implement databae interact */
	cout << endl;
	string primaryCommands = "1. Open Database   2. Create Database   3. About SQLight   4. Exit";
	while (true) {
		cout << primaryCommands << endl;
		int sel = 0;
		cout << " > " << flush;
		cin >> sel;
		switch (sel) {
		case 1:
			open_db();
		case 2:
			create_db();
			break;
		case 3:
			info();
			break;
		case 4:
			break;
		default:
			cout << "Undefined action" << endl;
		}
		if (sel == 4) break;
	}
	cout << "Good bye~" << endl;
	return 0;
}

int open_db()
{
	cin.ignore(1024, '\n');
	cout << "Input path of database: ";
	string path;
	getline(cin, path);
	DB db;
	try {
		db.open(path, 0);
	}
	catch (runtime_error err) {
		cout << err.what() << endl;
		return 1;
	}
	handle_db(db);
	return 0;
}

int create_db()
{
	/* 
	 * TODO 
	 * implement here
	 */
	cin.ignore(1024, '\n');
	cout << "Path to create a Database: " << flush;
	string path;
	getline(cin, path);
	DB db;
	try {
		db.open(path, 1);
	}
	catch (runtime_error err) {
		cout << err.what() << endl;
		return 1;
	}
	handle_db(db);
	return 0;
}

int info()
{
	cout << ">>        SQLight          <<\n" << "\tHelp Doc\n"
		<< "This is a light-wighted database, Designed by Higher Stark.\n"
		<< "Enter number for specified action.\n"
		<< "1. Open Database\n |-1. Store\n |-2.Fetch\n |-3.Delete\n |-4.Sync\n |-5.Close Database\n"
		<< "2.Create Database\n"
		<< "3.About SQLight\n"
		<< "4.Exit\n"
		<< "\t\t\t\t\t--Higher Stark\n"
		<< "\t\t\t\t\t2017/7/28\n"
		<< ">>        SQLight          <<"
		<< endl;
	return 0;
}

int handle_db(DB& db)
{
	while (true) {
		cout << "|- 1. Store   2. Fetch   3. Delete   4. Sync   5. Close Database\n" << endl;
		cout << " > " << flush;
		int sel = 0;
		cin >> sel;
		switch (sel) {
		case 1: {
			cout << "Key (No longer than 8 characters): ";
			Key key;
			cin >> key;
			while (key.size() > 8) {
				cout << "Invalid key" << endl;
				cout << "Key (No longer than 8 characters): ";
				cin >> key;
			}
			cout << "Data (input EOF at the end of input, in a new line): ";
			Data data;
			Data tmp;
			while (true) {
				getline(cin, tmp);
				if (cin.eof()) {
					cin.clear();
					break;
				}
				data += tmp + "\n";
			}
			try {
				db.store(key, data);
			}
			catch (runtime_error err) {
				cout << err.what() << endl;
				break;
			}
			break;
		}
		case 2: {
			cout << "Key (No longer than 8 characters): ";
			Key key;
			cin >> key;
			while (key.size() > 8) {
				cout << "Invalid key" << endl;
				cout << "Key (No longer than 8 characters): ";
				cin >> key;
			}
			cout << db.fetch(key) << endl;
			break;
		}
		case 3: {
			cout << "Key (No longer than 8 characters): ";
			Key key;
			cin >> key;
			while (key.size() > 8) {
				cout << "Invalid key" << endl;
				cout << "Key (No longer than 8 characters): ";
				cin >> key;
			}
			if (db.del(key)) {
				cout << "Delete successfully" << endl;
			}
			break;
		}
		case 4:
			db.sync();
			break;
		case 5:
			db.close();
			return 0;
		default:
			cout << "Undefined action" << endl;
		}
	}
}