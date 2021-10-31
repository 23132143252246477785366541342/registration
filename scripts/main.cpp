#include "sqlite3.h"
#include <string>
#include <termios.h>
#include <iostream>
#include <ctime>

using namespace std;

int getch() {
    struct termios oldtc, newtc;
    int ch;
    tcgetattr(fileno(stdin), &oldtc);
    newtc = oldtc;
    newtc.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &newtc);
    ch = getchar();
    tcsetattr(fileno(stdin), TCSANOW, &oldtc);
    return ch;
}

string passwordInput() {
    string password;
    int ch;
    bool tried = false;
    while (password.length() < 8 or password.length() > 20) {
        password = "";
        printf("Enter a password(minimum 8 characters, maximum 20 characters): ");
        for (int i = 0 ;; i++) {
            if (ch == 10 && i > 1)
                break;
            ch = getch();
            password += ch;
            if (ch == 127) {
                password.pop_back();
                password.pop_back();
            }
        }
        if (!tried) {
            password.erase(0, 1);
            tried = true;
        }

        password.pop_back();
        printf("\n");
    }
    return password;
}



int op = 0;
string selecta = "";
int selectOne(void *NotUsed, int argc, char **argv, char **azColName){

    selecta = argv[op];

    return 0;
}

int selectAll(void *NotUsed, int argc, char **argv, char **azColName){
    return 0;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName){

    // int argc: holds the number of results
    // (array) azColName: holds each column returned
    // (array) argv: holds each value

    for(int i = 0; i < argc; i++) {
        
        // Show column name, value, and newline
        printf("%s%s%s\n", azColName[i], ": ", argv[i]);
    
    }

    // Insert a newline
    printf("\n");

    // Return successful
    return 0;
}

int main() {
    sqlite3* db;
    int res = sqlite3_open("../databases/accounts.db", &db);
    while (true) {
        sqlite3* db;
        string sql;
        string username = "";
        string password = "";

        int res = sqlite3_open("../databases/accounts.db", &db);
        if(res) {
            //database failed to open
        }
        else {
            string option;
            printf("Enter an option:\n'o' to open an account\n'd' to delete an account\n's' to see an account's secret\n'a' to view all accounts(must be administrator)\n'q' to quit:\n");
            cin >> option;
            cout << endl;
            if (option == "o") {
                char *zErrMsg = 0;

                string secret = "";

                printf("Enter a username(minimum 4 characters, maximum 20 characters): ");
                cin >> username;

                password = passwordInput();

                printf("Enter a secret(Enter key means submit): ");
                cin >> secret;      
                cout << endl;
                sql = "CREATE TABLE IF NOT EXISTS ACCOUNTS("
                            "ID            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                            "Username      CHAR(30) NOT NULL,"
                            "Password      CHAR(30) NOT NULL,"
                            "Secret        CHAR(1000) NOT NULL);";
                
                res = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
                sql = "INSERT INTO ACCOUNTS(Username, Password, Secret) VALUES('" + username + "', '" + password + "', '" + secret + "');";
                res = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
                sqlite3_close(db);
            }
            else if (option == "d") {
                const char* data = "Callback function called";

                char *zErrMsg = 0;
                bool success = false;
                cout << "What is the username of the account? ";
                cin >> username;
                sql = "SELECT * FROM ACCOUNTS WHERE Username='" + username + "';";
                op = 1;
                res = sqlite3_exec(db, sql.c_str(), selectOne, (void*)data, &zErrMsg);
                if (selecta != username) {
                    cout << "No such account." << endl;
                }
                else {
                    for (int i = 5; i > 0; i--) {
                        if (i == 1) {
                            cout << "You have 1 try left" << endl;
                        }
                        else if (i != 1) {
                            printf("You have %d tries", i);
                            cout << endl;
                        }

                        password = passwordInput();
                        
                        op = 2;
                        res = sqlite3_exec(db, sql.c_str(), selectOne, (void*)data, &zErrMsg);
                        if (selecta == password) {
                            sql = "DELETE FROM ACCOUNTS WHERE Username='" + username + "';";
                            res = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
                            cout << "Delete Successful";
                            success = true;
                            break;
                        }
                    }
                    if (!success) {
                        cout << "You have failed to enter the correct password 5 times.";
                    }
                    cout << endl;
                }
            }
            else if (option == "a") {
                
            }
            else if (option == "q") {
                break;
            }
        }
        sqlite3_close(db);
    
    }
    
}