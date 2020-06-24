////
//// Created by Jonah Rubin on 2020-06-22.
////
//
//#include <stdint.h>
//#include <stdio.h>
//#include "Bus.h"
//#include <string.h>
//
//FILE *pathBuild(int i) {
//    char s1[60];
//    char s2[60];
//    char s3[60];
//
//    sprintf(s1, "%d", i);
//    sprintf(s2, "%s", "~/Users/jonah/CLionProjects/CommProtocol/p");
//    sprintf(s3, "%s", "Mail.txt");
//
//
//    strcat(s2, s1);
//
//    strcat(s2, s3);
//
//    FILE *fp = fopen(s2, "w");
//    return fp;
//}
//
//int transmit(char *data, Bus b) {
//    FILE *fp;
//
//    for (int i = 1; i <= sizeof(b.procIDs)/ sizeof(b.procIDs[0]); ++i) {
//
//        fp = pathBuild(i);
//
////        char s1[60];
////        char s2[60];
////        char s3[60];
////
////        sprintf(s1, "%d", i);
////        sprintf(s2, "%s", "../p");
////        sprintf(s3, "%s", "Mail.txt");
////
////        strcat(s2, s1);
////        strcat(s2, s3);
////        printf("%s\n", s2);
////
////        fp = fopen(s2, "w");
////
////        fprintf(fp, "This is aefqaewf for fprintf...\n");
////
////        fclose(fp);
//
//    }
//}
//
//int main(int argc, char *argv[]) {
//    Bus b = {.numProcs = 3,
//             .procIDs = {1,2,3},
//             .rem_reads = 3
//    };
//
//    char msg[] = "smem";
//
//    for (int i = 0; i < sizeof(msg)/ sizeof(msg[0]) - 1; ++i) {
//        transmit(&msg[i], b);
//    }
//}
//
//


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


#define BUFFER_SIZE 32
#define SEM_1_NAME "/SEM1_jhibbele"


char* isPalindrome(char *buffer) {
    for (int i = 0; i < strlen(buffer); ++i) {
        if (buffer[i] != buffer[strlen(buffer) - i - 1]) {
            return "no";
        }
    }
    return "yes";
}

int main(int argc, char *argv[]) {
    int pid;
    int memid;
    int key = 35;
    char *ptr;
    char buffer[BUFFER_SIZE];
    int done;
    int i, j;
    int rtnval;

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
        printf("(%d)\n", getpid());

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
        printf("I am the child, and my pid is %d\n", pid);
        ptr = (char *) shmat(memid, 0, 0);
        if (ptr == NULL) {
            printf("shmat() in child failed\n");
            return (8);
        }



        while (true) {
            if (strcmp(ptr, "end") == 0) exit(0);

            if (strcmp(ptr, "null") != 0) {
                // open pipe as write and send
                printf("(C) read this from the shared memory: '%s'\n", ptr);
                printf("(C) write this to the shared memory: '%s'\n", "null");
                strcpy(ptr, "null");
            } else {
                // open pipe as read
            }
        }
        return(0);
    }

    return(0);
}