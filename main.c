
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 32
#define PORT 8080

int make_server();

int main(int argc, char *argv[]) {
    int pid;
    int memid;
    int key = 31;
    char *ptr;
    char buffer[BUFFER_SIZE];
    int fd;
    // FIFO file path
    char * fifo = "/tmp/myfifo";
    char arr1[80], arr2[80];

    memid = shmget(key, BUFFER_SIZE, IPC_CREAT | 0666);
    if (memid < 0) {
        printf("shmget() failed\n");
        return(8);
    }

    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        return(8);
    }

    // this is the parent
    if (pid > 0) {

        // open shared memory
        ptr = (char *) shmat(memid, 0, 0);
        if (ptr == NULL) {
            printf("shmat() failed\n");
            return(8);
        }
        strcpy(ptr, "null");

        // continually check for new inputs
        while (true) {

            fgets(buffer, BUFFER_SIZE, stdin);
            strtok(buffer, "\n");

            if (strcmp(buffer, "end") == 0) exit(0);

            // wait and then place the input into the buffer
            sleep(1);
            strcpy(ptr, buffer);
        }
    } else {
        // this is the child
        pid = getpid();
        printf("I am on the network, my PID is %d\n", pid);

        // get shared memory
        ptr = (char *) shmat(memid, 0, 0);
        if (ptr == NULL) {
            printf("shmat() in child failed\n");
            return (8);
        }

        int new_socket, valread;
        char buffer2[1024] = {0};
        char *hello = "Hello from server";

        new_socket = make_server();

        // Creating the named FIFO (<pathname>, <permission>)
        mkfifo(fifo, 0666);
        while (true) {

            // check for ex
            if (strcmp(ptr, "end") == 0) exit(0);

            // if new input, switch to write
            if (strcmp(ptr, "null") != 0) {
                // Open FIFO for write only
                fd = open(fifo, O_WRONLY);

                // Write the input arr2ing on FIFO
                // and close it
                write(fd, ptr, strlen(ptr)+1);
                close(fd);
                strcpy(ptr, "null");
            } else {
                // if no new input, open FIFO for read only
                fd = open(fifo, O_RDONLY | O_NONBLOCK);
                sleep(1);

                // Read from FIFO
                ssize_t nread = read(fd, arr1, sizeof(arr1));

                if (nread > 0) {
                    // Print the read message
                    printf("User2: %s\n", arr1);
                }
                close(fd);

            }
        }
        return(0);
    }

    return(0);
}


int make_server() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}
