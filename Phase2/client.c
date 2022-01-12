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

    if (connect(svr->listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {   
        ERR_EXIT("Connection Failed");
    }

    return;
}

void login() {
    char buf[MAX_BUFFER_SIZE];

    // receive login message from server
    bzero(buf, MAX_BUFFER_SIZE);
    recv(svr.listen_fd, buf, sizeof(buf), 0);
    printf("%s", buf);

    // send username to server
    bzero(buf, MAX_BUFFER_SIZE);
    scanf("%s", buf);
    sprintf(user.username, "%s", buf);
    send(svr.listen_fd, buf, sizeof(buf), 0);

    return;
}

int main(int argc, char** argv) {

    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [ip]:[port]\n", argv[0]);
        exit(1);
    }

#ifdef DEBUG
    fprintf(stderr,"%s\n", argv[1]);
#endif

    char ip[48];
    unsigned short port;

    sscanf(argv[1], "%[^:]:%hd", ip, &port);

    // Initialize server
    init_server(&svr, port, ip);

    // login
    login();
}