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
#include <netinet/tcp.h>

// error handling and excit
#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)

#define MAX_BUFFER_SIZE 1024
#define MAX_CONN_FD 1024
#define MAX_LENGTH 256

char* INPUT_NAME = "input your username:\n";
char* NAME_SUCCESS = "connect successfully\n";
char* NAME_FAIL = "username is in used, please try another:\n";
char* COMMAND_ERR = "Command not found\n";
char* ACK = "ACK\n";

typedef struct {
    char hostname[MAX_LENGTH];  // server's hostname
    unsigned short port;  // port to listen
    int listen_fd;  // fd to wait for a new connection
} server;

typedef struct {
    char host[MAX_BUFFER_SIZE];  // client's host
    int port;
    int conn_fd;  // fd to talk with client
    char buf[MAX_BUFFER_SIZE];  // data sent by/to client
    char username[MAX_BUFFER_SIZE];
    size_t buf_len;  // bytes used by buf
    size_t username_len;
    int state;  // used to record the state of each users
} User;

enum userState {EMPTY, UNVERIFIED, VERIFIED};

// init user list
static void init_user(User* user) {
    user->conn_fd = -1;
    user->port = -1;
    user->buf_len = 0;
    user->state = EMPTY;
}

static void trim_space(char *str) {
    int start = 0, end = strlen(str) - 1;

    while (start < strlen(str) - 1 && str[start] != '\0' && isspace(str[start])) 
        start++;

    while (end >= start && isspace(str[end]))
        end--;

    for (int i = 0; i < end - start + 1; i++)
        str[i] = str[i + start];

    str[end - start + 1] = '\0';

    return;
}

// list all file under given directory
static void list(User* user) {
    struct dirent *pDirent;
    DIR *pDir;

    char arr[MAX_LENGTH][MAX_BUFFER_SIZE];
    int arrlen = 0;

    // open server_dir to read
    if ((pDir = opendir("./server_dir")) == NULL) {
        ERR_EXIT("opendir failed");
    }

    // Process each entry.
    while ((pDirent = readdir(pDir)) != NULL) {
        if (!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, ".."))
            continue;

        strcpy(arr[arrlen++], pDirent->d_name);
    }

    // sort array
    for(int i = 0; i < arrlen; i++){
        for(int j = 0; j < arrlen - 1 - i; j++){
            if(strcmp(arr[j], arr[j+1]) > 0){

                //swap array[j] and array[j+1]
                char tmp[MAX_BUFFER_SIZE];

                strcpy(tmp, arr[j]);
                strcpy(arr[j], arr[j + 1]);
                strcpy(arr[j + 1], tmp);
            }
        }
    }

    if (arrlen == 0) {
        send(user->conn_fd, ACK, strlen(ACK), 0);
        return;
    }

    // concat ls result into one string
    bzero(user->buf, MAX_BUFFER_SIZE);

    for (int i = 0; i < arrlen; i++) {
        strcat(user->buf, arr[i]);
        strcat(user->buf, "\n");
    }
#ifdef DEBUG
    fprintf(stderr, "%s\n", user->buf);
#endif

    send(user->conn_fd, user->buf, strlen(user->buf), 0);

    // Close directory and exit.
    closedir(pDir);
    return;
}

// send file api
static bool put_file(char* filepath, User* user) {

    // if file not exist
    if (access(filepath, R_OK) == -1) {
        send(user->conn_fd, "-1", strlen("-1"), 0);
        return false;
    }

    FILE *fp = fopen(filepath, "rb");

#ifdef DEBUG
    fprintf(stderr, "%s\n", filepath);
#endif

    if(fp == NULL)
        return false;

    // find the file length
    int fd = open(filepath, O_RDONLY);
    struct stat stbuf;

    if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) {
        fprintf(stderr, "fstat error\n");
    }

    off_t sz = stbuf.st_size;

    close(fd);

    char tmp_buffer[MAX_BUFFER_SIZE];

    // send the file length
    bzero(tmp_buffer, MAX_BUFFER_SIZE);
    sprintf(tmp_buffer, "%lld", sz);
    send(user->conn_fd, tmp_buffer, strlen(tmp_buffer), 0);

#ifdef DEBUG
    fprintf(stderr, "%lld\n", sz);
#endif

    // wait for awk message
    bzero(tmp_buffer, MAX_BUFFER_SIZE);
    recv(user->conn_fd, tmp_buffer, MAX_BUFFER_SIZE, 0);

    if (!sz)
        return true;

    while(sz) {

        bzero(user->buf, MAX_BUFFER_SIZE);
        int readlen = fread(user->buf, 1, MAX_BUFFER_SIZE, fp);

#ifdef DEBUG
        fprintf(stderr, "%d: %s", readlen, user->buf);
#endif

        if(send(user->conn_fd, user->buf, readlen, 0) == -1) {
            ERR_EXIT("Error in sending file");
        }

        sz -= readlen;
    }

    fclose(fp);

    // wait for awk message
    bzero(tmp_buffer, MAX_BUFFER_SIZE);
    recv(user->conn_fd, tmp_buffer, MAX_BUFFER_SIZE, 0);

    return true;
}

// get file api
static bool get_file(char* filepath, User* user) {

    char len[MAX_BUFFER_SIZE];
    bzero(len, MAX_BUFFER_SIZE);

    recv(user->conn_fd, len, MAX_BUFFER_SIZE, 0);

    off_t sz = (off_t)atoll(len);

#ifdef DEBUG
    fprintf(stderr, "%lld %d\n", sz, (sz == -1));
#endif

    if (sz == -1ll)
        return false;

    FILE *fp = fopen(filepath, "wb");

    if(fp == NULL) 
        ERR_EXIT("Fopen failed");

    send(user->conn_fd, ACK, sizeof(ACK), 0);

    if (!sz)
        return true;

    while(sz) {

        bzero(user->buf, MAX_BUFFER_SIZE);
        int readlen = recv(user->conn_fd, user->buf, MAX_BUFFER_SIZE, 0);

#ifdef DEBUG
        fprintf(stderr, "%d: %s\n", readlen, user->buf);
#endif

        fwrite(user->buf, readlen, 1, fp);

        sz -= readlen;
    }

    fclose(fp);

    send(user->conn_fd, ACK, sizeof(ACK), 0);

    return true;
}

bool check_command(char* input) {
    char buffer[MAX_BUFFER_SIZE], error_msg[MAX_BUFFER_SIZE];

    if (sscanf(input, "put %s %s", buffer, error_msg) == 1 || sscanf(input, "get %s %s", buffer, error_msg) == 1)
        return true;

    return false;
}
