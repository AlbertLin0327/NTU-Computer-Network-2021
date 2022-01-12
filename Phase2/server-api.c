#include "socket-api.c"

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