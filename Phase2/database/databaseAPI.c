#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "./sqlite3.h"

int check_callback(void *NotUsed, int argc, char **argv, char **azColName);
int getfriend_callback(void *NotUsed, int argc, char **argv, char **azColName);
int getmessage_callback(void *NotUsed, int argc, char **argv, char **azColName);
int getfile_callback(void *NotUsed, int argc, char **argv, char **azColName);
bool called = false;

char friends_list[100][100];
char friends_json[1000];
char message_list[100][1000];
char message_json[100000];
char path[1000];
int message_cnt = 0;
int friends_cnt = 0;

sqlite3* db;

int db_init(char *dbname){

    int rc = sqlite3_open(dbname, &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(db));
        sqlite3_close(db);   
        return 1;
    }
    return 0;
}

int db_adduser(char username[]){
    char sql[100];
    char* err_msg = 0;
    sprintf(sql, "INSERT INTO User VALUES('%s');", username);
    int rc = sqlite3_exec(db, sql, check_callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        if(rc == SQLITE_CONSTRAINT){
            return 2; // Duplicate name
        }else      
            return 1; // Other error
    } 
    return 0; // Success
}

bool db_checkuser(char username[]){
    char sql[100];
    char* err_msg = 0;
    sprintf(sql, "SELECT Name FROM User WHERE Name = '%s';", username);
    called = false;
    int rc = sqlite3_exec(db, sql, check_callback, 0, &err_msg);
    if (rc != SQLITE_OK || !called) {
        return false;
    } 
    return true; // Success
}

char* db_getuserfriend(char username[]){
    char sql[100];
    char* err_msg = 0;
    sprintf(sql, "SELECT Friend2 FROM Friendship WHERE Friend1 = '%s';", username);
    friends_cnt = 0;
    // bzero(friends_list, strlen(friends_list));
    int rc = sqlite3_exec(db, sql, getfriend_callback, 0, &err_msg);

    if (rc != SQLITE_OK || friends_cnt == 0) {
        return ""; // No friend
    }

    memset(friends_json, 0, sizeof(friends_json));
    
    friends_json[0] = '[';
    for(int i = 0; i < friends_cnt; i++){
        sprintf(friends_json + strlen(friends_json), "%s,", friends_list[i]);
    }
    sprintf(friends_json + strlen(friends_json), "]");
     
    return friends_json; // Success
}

int db_addfriendship(char username1[], char username2[]){
    if(!db_checkuser(username1) || !db_checkuser(username2))
        return 2;
    char sql[100];
    char* err_msg = 0;
    sprintf(sql, "INSERT INTO Friendship VALUES(NULL, '%s', '%s');", username2, username1);
    int rc = sqlite3_exec(db, sql, check_callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        if(rc == SQLITE_CONSTRAINT){
            return 2; // Duplicate name
        }else      
            return 1; // Other error
    } 
    sprintf(sql, "INSERT INTO Friendship VALUES(NULL, '%s', '%s');", username1, username2);
    rc = sqlite3_exec(db, sql, check_callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        if(rc == SQLITE_CONSTRAINT){
            return 2; // Duplicate name
        }else      
            return 1; // Other error
    }
    return 0; // Success
}

int db_deletefriendship(char username1[], char username2[]){
    if(!db_checkuser(username1) || !db_checkuser(username2))
        return 2;
    char sql[100];
    char* err_msg = 0;
    sprintf(sql, "DELETE FROM Friendship WHERE Friend1 = '%s' and Friend2 = '%s';", username2, username1);
    int rc = sqlite3_exec(db, sql, check_callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        if(rc == SQLITE_CONSTRAINT){
            return 2; // Duplicate name
        }else      
            return 1; // Other error
    }
    sprintf(sql, "DELETE FROM Friendship WHERE Friend1 = '%s' and Friend2 = '%s';", username1, username2);
    rc = sqlite3_exec(db, sql, check_callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        if(rc == SQLITE_CONSTRAINT){
            return 2; // Duplicate name
        }else      
            return 1; // Other error
    }
    return 0; 
}

char* db_getusermessage(char username1[], char username2[]){
    char sql[500];
    char* err_msg = 0;
    sprintf(sql, "SELECT * FROM Message WHERE (sender = '%s' and receiver = '%s') or (sender = '%s' and receiver = '%s');", username1, username2, username2, username1);
    message_cnt = 0;
    int rc = sqlite3_exec(db, sql, getmessage_callback, 0, &err_msg);

    if (rc != SQLITE_OK || message_cnt == 0) {
        return ""; // No friend
    }
    
    memset(message_json, 0, sizeof(message_json));
    message_json[0] = '[';
    for(int i = 0; i < message_cnt; i++){
        sprintf(message_json + strlen(message_json), "%s", message_list[i]);

    }
    sprintf(message_json + strlen(message_json), "]");
     
    return message_json; // Success
}

int db_addmessage(char sender[], char receiver[], int type, char content[]){
    if(!db_checkuser(sender) || !db_checkuser(receiver))
        return 2;
    char sql[1000];
    char* err_msg = 0;
    sprintf(sql, "INSERT INTO Message VALUES(NULL, '%s', '%s', %d, '%s');", sender, receiver, type, content);
    int rc = sqlite3_exec(db, sql, check_callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        if(rc == SQLITE_CONSTRAINT){
            return 2; // Duplicate name
        }else      
            return 1; // Other error
    }
    return 0; 
}

char* db_getfile(int id){
    char sql[1000];
    char* err_msg = 0;
    sprintf(sql, "SELECT content FROM Message WHERE id = %d;", id);
    memset(path, 0, sizeof(path));
    int rc = sqlite3_exec(db, sql, getfile_callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        return "";
    }
    return path; 
}

int check_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    called = true;
    return 0;
}

int getfriend_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    sprintf(friends_list[friends_cnt], "%s", argv[0]);
    friends_cnt++;
    // printf("%s\n", friends_list[friends_cnt - 1]);
    return 0;
}

int getmessage_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    // printf("%s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
    sprintf(message_list[message_cnt], "%sß%sß%sß%sß%s√", argv[0], argv[1], argv[2], argv[3], argv[4]);
    message_cnt++;
    // printf("1: %s\n", message_list[message_cnt - 1]);
    return 0;
}

int getfile_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    sprintf(path, "%s", argv[0]);
    return 0;
}

// int main(void) {
//     int rc = db_init("database.db");
//     // printf("%d\n", db_addmessage("Dino", "Hermes", 1, "Good night"));
//     // printf("%s\n", db_getusermessage("Dino", "Hermes"));
//     printf("%s\n", db_getfile(1));
//     printf("%s\n", db_getfile(2));
//     return 0;
// }
