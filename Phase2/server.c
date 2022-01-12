// C program for the Server Side
// Socket API file
#include "server-api.c"

server svr;  // server
pthread_t threadsArr[MAX_CONN_FD];  // pthread array
User userList[MAX_CONN_FD];  // user list
bool threadsUsed[MAX_CONN_FD]; // pthread array in used


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


    while (true) {
        switch (userList[thread_id].state) {
            case LOGIN:
                login(&userList[thread_id]);
                break;

            default:
                break;
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

	while (1) {

        // Check new connection
        clilen = sizeof(cliaddr);
        conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);

        // create new pthread
        threadsUsed[conn_fd] = true;

        // assign user information
        init_user(&userList[conn_fd]);
        userList[conn_fd].conn_fd = conn_fd;
        userList[conn_fd].state = LOGIN;

        int param = conn_fd;

        pthread_create(&threadsArr[conn_fd++], NULL, pthread_handler, (void *) (uintptr_t) param);
	}

	return 0;
}
