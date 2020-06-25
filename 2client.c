/**
    client
 */
#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include <zconf.h>
#include <sys/shm.h>
#include <sys/fcntl.h>


#define BUFFER_SIZE 1025

int main(int argc, char *argv[]) {
    int pid;
    int memid;
    int key = 10;
    char *ptr;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];


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
        while (1) {
            printf("\nInpit: ");
            fgets(message, BUFFER_SIZE, stdin);
            strtok(message, "\n");


            strcpy(ptr, message);
            printf("Wrote %s to ptr", ptr);
        }


    } else {
        int sock;
        int valread;
        struct sockaddr_in server;

        // this is the child
        //Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if (sock == -1) {
            printf("Could not create socket\n");
        }

        printf("Socket created\n");

        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_family = AF_INET;
        server.sin_port = htons(8888);

        //Connect to remote server
        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
            perror("connect failed. Error\n");
            return 1;
        }

        printf("Connected\n");

        // this is the child
        ptr = (char *) shmat(memid, 0, 0);
        if (ptr == NULL) {
            printf("shmat() in child failed\n");
            return (8);
        }



        //keep communicating with server
        while (1) {
            
            // here we can call not blockable read
            // if there is something to send
            if (strcmp(ptr, "null") != 0) {
                printf("(C) read this from the shared memory: '%s'\n", ptr);
                strcpy(ptr, "null");
            } else {
                // else continue reading
                //Receive a reply from the server
                valread = read(sock, server_reply, 1024);
                if (valread < 0) {
                    printf("recv failed");
                    break;
                } else if (valread > 0){
                    server_reply[valread] = '\0';

                    printf("Server reply: ");
                    printf("%s", server_reply);
                }
            }



//            //Receive a reply from the server
//            if ((valread = read(sock, server_reply, 1024)) < 0) {
//                printf("recv failed");
//                break;
//            }
//
//            server_reply[valread] = '\0';
//
//            printf("Server reply: ");
//            printf("%s", server_reply);
//
//            printf("Enter message: ");
//            fgets(message, BUFFER_SIZE, stdin);
//
//            //Send some data
//            if (send(sock, message, strlen(message), 0) < 0) {
//                printf("Send failed\n");
//                return 1;
//            }


        }

        close(sock);
    }
    return 0;
}