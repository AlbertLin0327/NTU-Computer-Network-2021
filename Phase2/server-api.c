#include "socket-api.c"
#include "database/databaseAPI.c"


void login(User* user) {

    // configure username query
    char text[MAX_BUFFER_SIZE];
    bzero(text, MAX_BUFFER_SIZE);
    sprintf(text, "Please Enter Your Username: ");

    // send username query to server
    send(user->conn_fd, text, MAX_BUFFER_SIZE, 0);

    // recieve username data
    bzero(text, MAX_BUFFER_SIZE);
    recv(user->conn_fd, text, MAX_BUFFER_SIZE, 0);

#ifdef DEBUG
    printf("%s\n", text);
#endif

    // configure username and state
    strcpy(user->username, text);
    user->state = HOMEPAGE;

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

    user->state = RETREIVEDHOMEPAGE;
}

void waiting_homepage(User* user) {
    bzero(user->buf, MAX_BUFFER_SIZE);
    recv(user->conn_fd, user->buf, MAX_BUFFER_SIZE, 0);

    if (!strncmp(user->buf, "ADD FRIEND:", 11)) {
        char newFriend[MAX_BUFFER_SIZE];
        strcpy(newFriend, user->buf + 11);

        db_addfriendship(user->username, newFriend);

        user->state = HOMEPAGE;
    
    } else if (!strncmp(user->buf, "CHAT:", 5)) {
        char newFriend[MAX_BUFFER_SIZE];
        strcpy(newFriend, user->buf + 5);

        

        strcpy(user->chatting, newFriend);
        user->state = CHAT;
    }
}