// Socket API file
#include "socket-api.c"
#include <string.h>

enum State {INIT, LOGIN, ADD, CHAT};
server svr_backend;
User user;
enum State state = INIT;

// initailize a server, exit for error
static void init_backend(unsigned short port, char* IPv4_address) {

    // defined in <netinet/in.h>
    struct sockaddr_in servaddr;
    int tmp;

    // get current host and port and record it with svr
    gethostname(svr_backend.hostname, sizeof(svr_backend.hostname));
    svr_backend.port = port;

    // establish TCP socket
    svr_backend.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_backend.listen_fd < 0) ERR_EXIT("socket creation failed");

    // initialized servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IPv4_address);
    servaddr.sin_port = htons(port); // bind to current port

    if (connect(svr_backend.listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {   
        ERR_EXIT("Connection Failed");
    }

    return;
}


    char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world~";

void login() {

    char buf[MAX_BUFFER_SIZE];
    bzero(buf, MAX_BUFFER_SIZE);
    bzero(user.username, MAX_BUFFER_SIZE);

    // receive login message from server
    printf("Username: ");
    scanf("%s", buf);

    sprintf(user.username, "%s", buf);
    bzero(buf, MAX_BUFFER_SIZE);

    sprintf(buf, "GET /user/%s HTTP/1.1\r\n", user.username);
    send(svr_backend.listen_fd, buf, sizeof(buf), 0);

    bzero(buf, MAX_BUFFER_SIZE);
    recv(svr_backend.listen_fd, buf, sizeof(buf), 0);
    
    char* result = strstr(buf, "\r\n\r\n") + 4;
    printf("Friends: %s\n", result);

    state = LOGIN;

    return;
}

void action() {
    char cmd[MAX_BUFFER_SIZE];
    char name[MAX_BUFFER_SIZE];
    bzero(cmd, MAX_BUFFER_SIZE);
    bzero(name, MAX_BUFFER_SIZE);

    // receive login message from server
    printf("Enter \'DEL [Name]\' OR \'ADD [Name]\' OR \'CHAT [Name]\': ");
    scanf("%s %s", cmd, name);

    printf("%s,%s\n",  cmd, name);

    if (!strcmp(cmd, "DEL")) {

    } else if (!strcmp(cmd, "ADD")) {

    } else if (!strcmp(cmd, "CHAT")) {

    }

    // bzero(buf, MAX_BUFFER_SIZE);
    // recv(svr_backend.listen_fd, buf, sizeof(buf), 0);
    
    // char* output = process(buf);
    // printf("%s\n", output);

    state = LOGIN;

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
    unsigned short port_backend, port_browser;

    sscanf(argv[1], "%[^:]:%hd", ip, &port_backend);

    // Initialize backend
    init_backend(port_backend, ip);

    char buffer[MAX_BUFFER_SIZE];

    while(true) {

        int backend_fd = svr_backend.listen_fd;

        switch(state) {
            case INIT:
                login();
                break;
            case LOGIN:
                action();
                break;
            case ADD:
                break;
            case CHAT:
                break;
        }
    }
}