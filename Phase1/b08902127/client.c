// Socket API file
#include "socket-api.c"

server svr; // server
User user;

// initailize a server, exit for error
static void init_server(server* svr, unsigned short port, char* IPv4_address) {

    // defined in <netinet/in.h>
    struct sockaddr_in servaddr;
    int tmp;

    // get current host and port and record it with svr
    gethostname(svr->hostname, sizeof(svr->hostname));
    svr->port = port;

    // establish TCP socket
    svr->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr->listen_fd < 0) ERR_EXIT("socket creation failed");

    // initialized servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IPv4_address);
    servaddr.sin_port = htons(port); // bind to current port

    // int temp = 1;
    // if (setsockopt(svr->listen_fd, IPPROTO_TCP, TCP_NODELAY, (void*)&temp, sizeof(temp)) < 0) {
    //     ERR_EXIT("setsockopt");
    // }

    if (connect(svr->listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {   
        ERR_EXIT("Connection Failed");
    }

    struct stat st = {0};
    
    if (stat("./client_dir", &st) == -1) {
        mkdir("./client_dir", 0744);
    }


    return;
}

int main(int argc, char** argv) {

    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [ip]:[port]\n", argv[0]);
        exit(1);
    }

    // create server directory
    struct stat st = {0};
    if (stat("./client_dir", &st) == -1) {
        mkdir("./client_dir", 0744);
    }

#ifdef DEBUG
    fprintf(stderr,"%s\n", argv[1]);
#endif

    char ip[48];
    unsigned short port;

    sscanf(argv[1], "%[^:]:%hd", ip, &port);

    // Initialize server
    init_server(&svr, port, ip);

    init_user(&user);

    user.conn_fd = svr.listen_fd;

    char buffer[MAX_BUFFER_SIZE];

    while (1) {

        // reinitialize buffer
        bzero(buffer, MAX_BUFFER_SIZE);

        if (user.state != VERIFIED) {
            recv(svr.listen_fd, buffer, MAX_BUFFER_SIZE, 0);
            fputs(buffer, stdout);

            if (!strcmp(buffer, NAME_SUCCESS)) {
                user.state = VERIFIED;
                continue;
            }

            fgets(buffer, MAX_BUFFER_SIZE, stdin);
            trim_space(buffer);

#ifdef DEBUG
        fprintf(stderr, "%s\n", buffer);
#endif

            send(svr.listen_fd, buffer, sizeof(buffer), 0);

        } else if (user.state == VERIFIED) {

            bzero(buffer, MAX_BUFFER_SIZE);
            fgets(buffer, MAX_BUFFER_SIZE, stdin);
            trim_space(buffer);

            send(svr.listen_fd, buffer, sizeof(buffer), 0);

            if (strlen(buffer) == 2 && !strcmp(buffer, "ls")) {
                
                bzero(buffer, MAX_BUFFER_SIZE);
                recv(svr.listen_fd, buffer, MAX_BUFFER_SIZE, 0);

                if (strcmp(buffer, ACK))
                    fputs(buffer, stdout);
                

            } else if (check_command(buffer) && !strncmp(buffer, "get", 3)) {

                char filename[MAX_BUFFER_SIZE];
                bzero(filename, MAX_BUFFER_SIZE);
                sscanf(buffer, "get %s", filename);
                
                char filepath[MAX_BUFFER_SIZE];
                bzero(filepath, MAX_BUFFER_SIZE);
                sprintf(filepath, "./client_dir/%s", filename);

                bzero(buffer, MAX_BUFFER_SIZE);

                if (!get_file(filepath, &user)) {
                    bzero(buffer, MAX_BUFFER_SIZE);
                    sprintf(buffer, "The %s doesn’t exist\n", filename);

                } else {
                    bzero(buffer, MAX_BUFFER_SIZE);
                    sprintf(buffer, "get %s successfully\n", filename);
                }

                fputs(buffer, stdout);

            } else if (check_command(buffer) && !strncmp(buffer, "put", 3)) {

                char filename[MAX_BUFFER_SIZE];
                bzero(filename, MAX_BUFFER_SIZE);
                sscanf(buffer, "put %s", filename);

                char filepath[MAX_BUFFER_SIZE];
                bzero(filepath, MAX_BUFFER_SIZE);
                sprintf(filepath, "./client_dir/%s", filename);

                // wait for awk message
                bzero(buffer, MAX_BUFFER_SIZE);
                recv(user.conn_fd, buffer, MAX_BUFFER_SIZE, 0);

#ifdef DEBUG
                fprintf(stderr, "%s\n", buffer);
#endif

                bzero(buffer, MAX_BUFFER_SIZE);

                if (!put_file(filepath, &user)) {
                    bzero(buffer, MAX_BUFFER_SIZE);
                    sprintf(buffer, "The %s doesn’t exist\n", filename);

                } else {
                    bzero(buffer, MAX_BUFFER_SIZE);
                    sprintf(buffer, "put %s successfully\n", filename);
                }

                fputs(buffer, stdout);

            } else {
                bzero(buffer, MAX_BUFFER_SIZE);
                recv(svr.listen_fd, buffer, MAX_BUFFER_SIZE, 0);
                fputs(buffer, stdout);
            }
        }
    }
}