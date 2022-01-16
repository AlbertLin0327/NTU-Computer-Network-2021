// import necessary library
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <pthread.h>

// error handling and excit
#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)

#define MAX_BUFFER_SIZE 1024
#define MAX_CONN_FD 1024
#define MAX_LENGTH 256

typedef struct {
    char hostname[MAX_LENGTH];  // server's hostname
    unsigned short port;  // port to listen
    int listen_fd;  // fd to wait for a new connection
} server;

typedef struct {
    int conn_fd;  // fd to talk with client
    char buf[MAX_BUFFER_SIZE];  // data sent by/to client
    char username[MAX_BUFFER_SIZE];
    char chatting[MAX_BUFFER_SIZE];
    size_t buf_len;  // bytes used by buf
    size_t username_len;
} User;

// init user list
static void init_user(User* user) {
    bzero(&user->buf, sizeof(user->buf));
    bzero(&user->username, sizeof(user->username));
    user->conn_fd = -1;
    user->buf_len = 0;
}

