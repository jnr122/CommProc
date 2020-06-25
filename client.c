/*
	C ECHO client example using sockets
*/
#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include <zconf.h>

#define BUFFER_SIZE 1025

int main(int argc, char *argv[]) {
    int sock;
    int valread;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

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
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error\n");
        return 1;
    }

    printf("Connected\n");

    //keep communicating with server
    while(1) {

        //Receive a reply from the server
        if ((valread = read(sock, server_reply, 1024)) < 0) {
            printf("recv failed");
            break;
        }

        server_reply[valread] = '\0';

        printf("Server reply: ");
        printf("%s", server_reply);

        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);

        //Send some data
        if (send(sock, message, strlen(message), 0) < 0) {
            printf("Send failed\n");
            return 1;
        }


    }

    close(sock);
    return 0;
}