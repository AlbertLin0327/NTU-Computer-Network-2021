// Socket API file
#include "socket-api.c"
#include <string.h>

server svr_browser, svr_cmd, svr_backend;
User user;

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

// Initialize Server
static void init_browser(unsigned short port) {
    struct sockaddr_in servaddr;

    // Server Configuration
    gethostname(svr_browser.hostname, sizeof(svr_browser.hostname));
    svr_browser.port = port;

    // Register for socket
    svr_browser.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_browser.listen_fd < 0) ERR_EXIT("socket");
    
    // configure socket parameters
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Bind Socket
    if (bind(svr_browser.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) 
        ERR_EXIT("bind");
    
    // Listen Socket
    if (listen(svr_browser.listen_fd, 1024) < 0) 
        ERR_EXIT("listen");

    return;
}

void login() {
    char buf[MAX_BUFFER_SIZE];

    // receive login message from server
    bzero(buf, MAX_BUFFER_SIZE);
    recv(svr_backend.listen_fd, buf, sizeof(buf), 0);

    // send username to server
    bzero(buf, MAX_BUFFER_SIZE);
    scanf("%s", buf);
    sprintf(user.username, "%s", buf);
    send(svr_backend.listen_fd, buf, sizeof(buf), 0);

    return;
}

void homepage() {
    
}

int main(int argc, char** argv) {

    // Parse args.
    if (argc != 3) {
        fprintf(stderr, "usage: %s [ip]:[port] [port]\n", argv[0]);
        exit(1);
    }

#ifdef DEBUG
    fprintf(stderr,"%s\n", argv[1]);
#endif

    char ip[48];
    unsigned short port_backend, port_browser;

    sscanf(argv[1], "%[^:]:%hd", ip, &port_backend);
    sscanf(argv[2], "%hd", &port_browser);

    // Initialize browser
    init_browser(port_browser);
    

    // Initialize backend
    init_backend(port_backend, ip);

    char buffer[MAX_BUFFER_SIZE];

    while(true) {

        int browser_fd = accept(svr_browser.listen_fd, NULL, NULL);
        int backend_fd = svr_backend.listen_fd;

        bool first_pkt = true;
        int total_len = 0;  

        do{
            // first pkt
            bzero(buffer, MAX_BUFFER_SIZE);  
            int readlen = recv(browser_fd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
            
            // find length
            if (first_pkt && !strncmp(buffer, "POST", 4)) {
                char* content_len = strstr(buffer, "Content-Length: ") + 15;
                char* content = strstr(buffer, "\r\n\r\n") + 4;
                
                sscanf(content_len, "%d", &total_len);
#ifdef DEBUG
                printf("%d %d\n", total_len, strlen(content));
#endif

                total_len -= strlen(content);

            } else total_len -= readlen;

            send(backend_fd, buffer, strlen(buffer), 0); 
            first_pkt = false;

#ifdef DEBUG
            printf("%d\n", total_len);
#endif
           
        } while(total_len > 0);

        first_pkt = true;
        bzero(buffer, MAX_BUFFER_SIZE);

        do{
            // first pkt
            int readlen = recv(backend_fd, buffer, sizeof(buffer), 0);

#ifdef DEBUG
            printf("%s\n", buffer);
#endif
            
            // find length
             if (first_pkt) {
                char* content_len = strstr(buffer, "Content-Length: ") + 15;
                char* content = strstr(buffer, "\r\n\r\n") + 4;
                
                sscanf(content_len, "%d", &total_len);
                total_len -= strlen(content);

            } else total_len -= readlen;

            send(browser_fd, buffer, strlen(buffer), 0);
            first_pkt = false;

#ifdef DEBUG
            printf("%s\n", buffer);
#endif
           
        } while(total_len > 0);

    }
}