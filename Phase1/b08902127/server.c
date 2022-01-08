// Socket API file
#include "socket-api.c"

int fd_range;
int currentfd;
server svr;  // server
User userList[MAX_CONN_FD];  // user list
fd_set master_read, readfds;  // fd_set to control I/O multiplexing

// Initialized user list
static void init_userList() {

    for (int i = 0; i < MAX_CONN_FD; i++)
        init_user(&userList[i]);
    
    // initialized server
    userList[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(userList[svr.listen_fd].host, svr.hostname);

    return;
}

// initialize server
static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;

    // server configuration
    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // int temp = 1;
    // if (setsockopt(svr.listen_fd, IPPROTO_TCP, TCP_NODELAY, (void*)&temp, sizeof(temp)) < 0) {
    //     ERR_EXIT("setsockopt");
    // }

    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) 
        ERR_EXIT("bind");
    
    if (listen(svr.listen_fd, 1024) < 0) 
        ERR_EXIT("listen");

    init_userList();

    struct stat st = {0};

    if (stat("./server_dir", &st) == -1) {
        mkdir("./server_dir", 0744);
    }


    return;
}

static void disconnect(int sockfd){

#ifdef DEBUG
    fprintf(stderr,"disconnected fd: %d\n", sockfd);
#endif

    FD_CLR(sockfd, &master_read);
    init_user(&userList[sockfd]);
    close(sockfd);
}

static void sigpipe_handler(int signum){
    disconnect(currentfd);
    return;
}

// handle incomming commnad
int handle_command(int sockfd) {

    // recive user command
    bzero(userList[sockfd].buf, MAX_BUFFER_SIZE);
    recv(userList[sockfd].conn_fd, userList[sockfd].buf, MAX_BUFFER_SIZE, 0);

    // post-processing
    trim_space(userList[sockfd].buf);

    // registration
    if (userList[sockfd].state == UNVERIFIED) {
        
        // not empty
        bool legal = (strlen(userList[sockfd].buf) > 0);

        // check for reconnection
        for (int j = 3; j <= fd_range; j++) {
            if (userList[j].state == VERIFIED && !strcmp(userList[j].username, userList[sockfd].buf)) {
                legal = false;
                break;
            }
        }

        // setup new connection profile
        if (legal) {
            strcpy(userList[sockfd].username, userList[sockfd].buf);
            userList[sockfd].state = VERIFIED;

            // send success message
            send(userList[sockfd].conn_fd, NAME_SUCCESS, strlen(NAME_SUCCESS), 0);
        
        } else {

            // login fail
            send(userList[sockfd].conn_fd, NAME_FAIL, strlen(NAME_FAIL), 0);
        }
    
    } else if (userList[sockfd].state == VERIFIED) {
                        
#ifdef DEBUG
        fprintf(stderr, "%s\n", userList[sockfd].buf);
#endif

        // handle "ls"
        if (strlen(userList[sockfd].buf) == 2 && !strcmp(userList[sockfd].buf, "ls")) {
            list(&userList[sockfd]);
        
        // handle "get file"
        } else if (check_command(userList[sockfd].buf) && !strncmp(userList[sockfd].buf, "get", 3)) {
            
            // parse correct file path
            char filename[MAX_BUFFER_SIZE];
            bzero(filename, MAX_BUFFER_SIZE);
            sscanf(userList[sockfd].buf, "get %s", filename);
            
            char filepath[MAX_BUFFER_SIZE];
            bzero(filepath, MAX_BUFFER_SIZE);
            sprintf(filepath, "./server_dir/%s", filename);

#ifdef DEBUG
            fprintf(stderr, "%s\n", userList[sockfd].buf);
#endif

            // prepare to send file
            put_file(filepath, &userList[sockfd]);

        } else if (check_command(userList[sockfd].buf) && !strncmp(userList[sockfd].buf, "put", 3)) {
            
             // parse correct file path
            char filename[MAX_BUFFER_SIZE];
            bzero(filename, MAX_BUFFER_SIZE);
            sscanf(userList[sockfd].buf, "put %s", filename);
            
            char filepath[MAX_BUFFER_SIZE];
            bzero(filepath, MAX_BUFFER_SIZE);
            sprintf(filepath, "./server_dir/%s", filename);

            // send awk
            send(userList[sockfd].conn_fd, ACK, sizeof(ACK), 0);

            // prepare to get file
            get_file(filepath, &userList[sockfd]);

        } else send(userList[sockfd].conn_fd, COMMAND_ERR, strlen(COMMAND_ERR), 0);
    }

    return 1;
}

int main(int argc, char** argv) {

    struct sockaddr_in cliaddr;  // used by accept()
    int clilen, conn_fd;  // new connection with client

    // initialize fd_set
    FD_ZERO(&master_read);

    // Initialize server
    init_server((unsigned short) atoi(argv[1]));

#ifdef DEBUG
    // Loop for handling connections
    fprintf(stderr, "\nstarting on %.80s, port %d, fd %d, maxconn %d...\n", svr.hostname, svr.port, svr.listen_fd, MAX_CONN_FD);
#endif

    // the range of fd, and currently file_fd is the largest one.
    fd_range = svr.listen_fd;
    struct timeval timeout = {5, 0};

    // SIGPIPE generates when one side of the socket connection is broken. 
    signal(SIGPIPE, sigpipe_handler);

    while (1) {

        // renew fd_set to master
        FD_SET(svr.listen_fd, &master_read);
        readfds = master_read;

        // Use select for I/O Multiplexing
        int totalFds = select(fd_range + 1, &readfds, NULL, NULL, &timeout);

        if(totalFds == -1)
            fprintf(stderr, "select failed: %s\n", strerror(errno));
        else if(totalFds == 0)
            fprintf(stderr, "select timeout\n");

        for(currentfd = 3; currentfd <= fd_range; currentfd++){

#ifdef DEBUG
            fprintf(stderr, "current: %d, %d\n", currentfd, FD_ISSET(currentfd, &readfds));
#endif

            // if available for reading
            if(FD_ISSET(currentfd, &readfds)){

                // if a new socket is available (list on svr.listen_fd)
                if(currentfd == svr.listen_fd){

                    // Check new connection
                    clilen = sizeof(cliaddr);
                    conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);

                    if (conn_fd < 0) {
                        if (errno == EINTR || errno == EAGAIN) continue;  // try again
                        if (errno == ENFILE) {
                            (void) fprintf(stderr, "out of file descriptor table ... (maxconn %d)\n", MAX_CONN_FD);
                            continue;
                        }
                        ERR_EXIT("accept");
                    }

                    // configure new conncetion
                    userList[conn_fd].conn_fd = conn_fd;
                    strcpy(userList[conn_fd].host, inet_ntoa(cliaddr.sin_addr));

#ifdef DEBUG
                    fprintf(stderr, "getting a new request... fd %d from %s\n", conn_fd, userList[conn_fd].host);
#endif

                    // renew the range if needed
                    if(conn_fd > fd_range)
                        fd_range = conn_fd;

                    // initialize and wait for the server to write to the socket
                    send(conn_fd, INPUT_NAME, strlen(INPUT_NAME), 0);
                    userList[conn_fd].state = UNVERIFIED;
                    
                    FD_SET(conn_fd, &master_read);

                } else {

                    // handle client command
                    handle_command(currentfd);
                }

            }
        }
    }

}