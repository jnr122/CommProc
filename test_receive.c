
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

#define PORT 8080



#define BUFFER_SIZE 32

int main(int argc, char *argv[]) {
    int pid;
    int memid;
    int key = 35;
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

    if (pid > 0) {
        // this is the parent
        ptr = (char *) shmat(memid, 0, 0);
        if (ptr == NULL) {
            printf("shmat() failed\n");
            return(8);
        }
        strcpy(ptr, "null");

        while (true) {

            // send messages to be transmitted here
            fgets(buffer, BUFFER_SIZE, stdin);
            strtok(buffer, "\n");

            if (strcmp(buffer, "end") == 0) exit(0);

            sleep(1);
            strcpy(ptr, buffer);
        }
    }
    else {
        // this is the child
        pid = getpid();
        printf("I am on the network, and my pid is %d\n", pid);
        ptr = (char *) shmat(memid, 0, 0);
        if (ptr == NULL) {
            printf("shmat() in child failed\n");
            return (8);
        }

        while (true) {

            if (strcmp(ptr, "end") == 0) exit(0);

            if (strcmp(ptr, "null") != 0) {
                // open pipe as write and send
                printf("MESSAGE: '%s'\n", ptr);

                strcpy(ptr, "null");
            } else {
                // Open FIFO for Read only
                fd = open(fifo, O_RDONLY | O_NONBLOCK);

                // Read from FIFO
                int nread = read(fd, arr1, sizeof(arr1));

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