// C program for the Server Side
// Socket API file
#include "socket-api.c"

#define THREAD_NUM 1024
server svr;  // server
pthread_t threadsArr[THREAD_NUM];  // pthread array
bool threadsUsed[THREAD_NUM]; // pthread array in used

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

void* pthread_handler(void* data) {

    // retrieve thread id
    int thread_id = (int) data;

    printf("%d\n", thread_id);

    sleep(10);
    

    // end pthread
    threadsUsed[thread_id] = false;
    pthread_exit(NULL); // 離開子執行緒
}

int nextThread() {
    for (int i = 0; i < THREAD_NUM; i++) {
        if (!threadsUsed[i])
            return i;
    }

    return 0;
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

        printf("ACCEPT %d\n", conn_fd);

        // create new pthread
        int thread_id = nextThread();
        threadsUsed[thread_id] = true;
        pthread_create(&threadsArr[thread_id++], NULL, pthread_handler, (void *) thread_id);

	}

	return 0;
}
