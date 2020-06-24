
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

#define BUFFER_SIZE 32

int main(int argc, char *argv[]) {
    int pid;
    int memid;
    int key = 55;
    char *ptr;
    char buffer[BUFFER_SIZE];
    int fd;
    // FIFO file path
    char * fifo = "/tmp/myfifo";
    char str1[80], str2[80];

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

            fgets(buffer, BUFFER_SIZE, stdin);
            strtok(buffer, "\n");

            if (strcmp(buffer, "end") == 0) exit(0);

            printf("(P) writing '%s' to the shared memory\n", buffer);
            sleep(2);
            strcpy(ptr, buffer);
        }
    }
    else {
        // this is the child
        pid = getpid();
        printf("I am on the network, my PID is %d\n", pid);
        ptr = (char *) shmat(memid, 0, 0);
        if (ptr == NULL) {
            printf("shmat() in child failed\n");
            return (8);
        }



        // Creating the named file(FIFO)
        // mkfifo(<pathname>, <permission>)
        mkfifo(fifo, 0666);

        while (true) {

            if (strcmp(ptr, "end") == 0) exit(0);

            if (strcmp(ptr, "null") != 0) {
                // open pipe as write and send
                printf("(C) read this from the shared memory: '%s'\n", ptr);

                // Open FIFO for write only
                fd = open(fifo, O_WRONLY);

                // Write the input arr2ing on FIFO
                // and close it
                write(fd, ptr, strlen(ptr)+1);
                close(fd);

                printf("(C) write this to the shared memory: '%s'\n", "null");
                strcpy(ptr, "null");
            } else {
                // Open FIFO for Read only
                fd = open(fifo, O_RDONLY | O_NONBLOCK);

                // Read from FIFO
                ssize_t nread = read(fd, str1, sizeof(str1));
                sleep(2);

                if (nread > 0) {
                    // Print the read message
                    printf("User1: %s\n", str1);
                }
                close(fd);

            }
        }
        return(0);
    }

    return(0);
}