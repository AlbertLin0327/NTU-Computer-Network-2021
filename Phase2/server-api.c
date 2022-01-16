#include "socket-api.c"
#include "database/databaseAPI.c"


void login(User* user, char* username) {

    // configure username query
    char text[MAX_BUFFER_SIZE];
    bzero(text, MAX_BUFFER_SIZE);
    sprintf(text, "Please Enter Your Username: ");

    if(!db_checkuser(username)) {
        db_adduser(username);
    }

    char *friendList = db_getuserfriend(username);

// #ifdef DEBUG
    printf("%s\n", friendList);
// #endif

    char res[MAX_BUFFER_SIZE];
    sprintf(res, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s", strlen(friendList), friendList);

// #ifdef DEBUG
    printf("%s\n", res);
// #endif

    send(user->conn_fd, res, strlen(res), 0);


    return;
}

void deleteFriend(User* user, char* username, char* deleteName) {
    // configure username query
    char text[MAX_BUFFER_SIZE];
    bzero(text, MAX_BUFFER_SIZE);
    sprintf(text, "Please Enter Your Username: ");

    int ret = db_deletefriendship(username, deleteName);

    if (ret == 0) {
        char res[MAX_BUFFER_SIZE];
        sprintf(res, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nConnection: close\r\n\r\nOK");

// #ifdef DEBUG
        printf("%s\n", res);
// #endif

        send(user->conn_fd, res, strlen(res), 0);

    } else {
        char res[MAX_BUFFER_SIZE];
        sprintf(res, "HTTP/1.1 500 INTERNAL SERVER ERROR\r\nContent-Length: 5\r\nConnection: close\r\n\r\nERROR");

// #ifdef DEBUG
        printf("%s\n", res);
// #endif

        send(user->conn_fd, res, strlen(res), 0);
    }

    return;
}

void addFriend(User* user, char* username, char* friendName) {
    // configure username query
    char text[MAX_BUFFER_SIZE];
    bzero(text, MAX_BUFFER_SIZE);
    sprintf(text, "Please Enter Your Username: ");

    printf("%s %s", username, friendName);

    int ret = db_addfriendship(username, friendName);

    if (ret == 0) {
        char res[MAX_BUFFER_SIZE];
        sprintf(res, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nConnection: close\r\n\r\nOK");

// #ifdef DEBUG
        printf("%s\n", res);
// #endif

        send(user->conn_fd, res, strlen(res), 0);

    } else {
        char res[MAX_BUFFER_SIZE];
        sprintf(res, "HTTP/1.1 500 INTERNAL SERVER ERROR\r\nContent-Length: 5\r\nConnection: close\r\n\r\nERROR");

// #ifdef DEBUG
        printf("%s\n", res);
// #endif

        send(user->conn_fd, res, strlen(res), 0);
    }

    return;
}

void homepage(User* user) {

    char* list = db_getuserfriend(user->username);

    int sz = strlen(list);
    char length[MAX_BUFFER_SIZE];

    bzero(length, sizeof(length));
    send(user->conn_fd, length, MAX_BUFFER_SIZE, 0);

    bzero(user->buf, MAX_BUFFER_SIZE);
    recv(user->conn_fd, user->buf, MAX_BUFFER_SIZE, 0);

    while(sz) {

        bzero(user->buf, MAX_BUFFER_SIZE);
        int readlen;

#ifdef DEBUG
        fprintf(stderr, "%d: %s", readlen, user->buf);
#endif

        if((readlen = send(user->conn_fd, user->buf, readlen, 0)) == -1) {
            ERR_EXIT("Error in sending file");
        }

        sz -= readlen;
    }

    bzero(user->buf, MAX_BUFFER_SIZE);
    recv(user->conn_fd, user->buf, MAX_BUFFER_SIZE, 0);

}

void waiting_homepage(User* user) {
    bzero(user->buf, MAX_BUFFER_SIZE);
    recv(user->conn_fd, user->buf, MAX_BUFFER_SIZE, 0);

    if (!strncmp(user->buf, "ADD FRIEND:", 11)) {
        char newFriend[MAX_BUFFER_SIZE];
        strcpy(newFriend, user->buf + 11);

        db_addfriendship(user->username, newFriend);

    
    } else if (!strncmp(user->buf, "CHAT:", 5)) {
        char newFriend[MAX_BUFFER_SIZE];
        strcpy(newFriend, user->buf + 5);

        

        strcpy(user->chatting, newFriend);
     
    }
}