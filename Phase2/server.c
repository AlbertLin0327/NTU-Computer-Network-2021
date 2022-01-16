// C program for the Server Side
// Socket API file
#include "server-api.c"
#include <string.h>

server svr;  // server
pthread_t threadsArr[MAX_CONN_FD];  // pthread array
User userList[MAX_CONN_FD];  // user list
bool threadsUsed[MAX_CONN_FD]; // pthread array in used
char buffer[1<<20];


// Initialize Server
static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;

    // Server Configuration
    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    // Register for socket
    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");
    
    // configure socket parameters
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Bind Socket
    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) 
        ERR_EXIT("bind");
    
    // Listen Socket
    if (listen(svr.listen_fd, 1024) < 0) 
        ERR_EXIT("listen");

    return;
}

// Initialized user list
static void init_userList() {

    for (int i = 0; i < MAX_CONN_FD; i++)
        init_user(&userList[i]);
    
    // initialized server
    userList[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(userList[svr.listen_fd].username, "server");

    return;
}

void* pthread_handler(void* data) {

    // retrieve thread id
    int thread_id = (int) data;

    printf("ACCEPT %d\n", thread_id);
    char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world~";

    while(1){

        char tmp_buf[MAX_BUFFER_SIZE];
        bzero(buffer, sizeof(buffer));
        bzero(tmp_buf, sizeof(tmp_buf));
        

        bool first_pkt = true;
        int total_len = 0;  

        do{
            // first pkt
            int readlen = recv(userList[thread_id].conn_fd, tmp_buf, sizeof(tmp_buf), 0);
            printf("%s\n", tmp_buf);
            
            // find length
            if (first_pkt && !strncmp(tmp_buf, "POST", 4)) {
                char* content_len = strstr(tmp_buf, "Content-Length: ") + 15;
                char* content = strstr(tmp_buf, "\r\n\r\n") + 4;
                
                sscanf(content_len, "%d", &total_len);
                printf("%d %d\n", total_len, strlen(content));

                total_len -= strlen(content);

            } else total_len -= readlen;

            first_pkt = false;

            printf("%d\n", total_len);
            strcat(buffer, tmp_buf);
           
        } while(total_len > 0);


        if (!strncmp(buffer, "GET /user/", 10)) {
            char username[MAX_BUFFER_SIZE];
            sscanf(buffer, "GET /user/%s", username);

            login(&userList[thread_id], username);
        
        } else if (!strncmp(buffer, "POST /user/delete/", 18)) {
            char username[MAX_BUFFER_SIZE];
            sscanf(buffer, "POST /user/delete/%s", username);

            char* deleteName = strstr(buffer, "\r\n\r\n") + 4;

            printf("%s\n", deleteName);

            deleteFriend(&userList[thread_id], username, deleteName);
        
        } else if (!strncmp(buffer, "POST /user/add/", 15)) {
            char username[MAX_BUFFER_SIZE];
            sscanf(buffer, "POST /user/add/%s", username);

            char* friendName = strstr(buffer, "\r\n\r\n") + 4;

            printf("%s\n", friendName);

            addFriend(&userList[thread_id], username, friendName);
        }

        
    }


    // end pthread
    threadsUsed[thread_id] = false;
    pthread_exit(NULL);
}

// Driver Code
int main(int argc, char** argv) {

    if (argc != 2) ERR_EXIT("#arguments");

	struct sockaddr_in cliaddr;  // used by accept()
    int clilen, conn_fd;  // new connection with client

    // // Initialize server
    init_server((unsigned short) atoi(argv[1]));

    // Initialize DB
    int rc = db_init("database/database.db");

	while (1) {

        // Check new connection
        clilen = sizeof(cliaddr);
        printf("%d", svr.listen_fd);
        conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);

        // create new pthread
        threadsUsed[conn_fd] = true;

        // assign user information
        init_user(&userList[conn_fd]);
        userList[conn_fd].conn_fd = conn_fd;

        int param = conn_fd;

        pthread_create(&threadsArr[conn_fd++], NULL, pthread_handler, (void *) (uintptr_t) param);
	}

	return 0;
}
