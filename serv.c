/**
 * multi client server
 */
#include <stdio.h>
#include <string.h>      //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>      //close
#include <arpa/inet.h>   //close
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>    //FD_SET, FD_ISSET, FD_ZERO macros
#include "serv.h"
#define TRUE   1
#define FALSE  0
#define PORT 8888
#define BUFFER_SIZE 1024

// initialize, set up and run
int main(int argc , char *argv[]) {

    Server s;
    s.max_clients = 30;
    s.message = "Welcome to the server \n";

    setup_server(&s);
    while(TRUE)
        run(&s);

    return 0;
}

/**
 * Run the server, listen for activity and respond
 * @param s the server struct
 * @return 0 on success
 */
int run(Server *s) {

    char temp[30];
    //clear the socket set
    FD_ZERO(&s->readfds);

    //add master socket to set
    FD_SET(s->master_socket, &s->readfds);
    s->max_sd = s->master_socket;

    //add child sockets to set
    for (int i = 0; i < s->max_clients; i++) {
        //socket descriptor
        s->sd = s->client_socket[i];

        //if valid socket descriptor then add to read list
        if (s->sd > 0)
            FD_SET(s->sd, &s->readfds);

        //highest file descriptor number, need it for the select function
        if (s->sd > s->max_sd)
            s->max_sd = s->sd;
    }

    // wait for an activity on one of the sockets, timeout is NULL
    s->activity = select(s->max_sd + 1, &s->readfds, NULL, NULL, NULL);

    if ((s->activity < 0) && (errno != EINTR))
        printf("select error");

    // if something happened on the master socket then its an incoming connection
    if (FD_ISSET(s->master_socket, &s->readfds)) {
        if ((s->new_socket = accept(s->master_socket,
                                    (struct sockaddr *) &s->address, (socklen_t *) &s->addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        //add new socket to array of sockets
        for (int i = 0; i < s->max_clients; i++) {
            //if position is empty
            if (s->client_socket[i] == 0) {
                s->client_socket[i] = s->new_socket;
                printf("Adding to list of sockets as %d\n", i);
                s->message = "i";
                snprintf(temp, sizeof(temp), "%d",i);
                s->message = temp;

                // here build packet for first connection, for now just send ID to master
                if (send(s->client_socket[0], s->message, strlen(s->message), 0) != strlen(s->message))
                    perror("send");
                break;
            }
        }

        // inform user of socket number - used in send and receive commands
        printf("New connection: socket fd is %d, ip is %s, port is %d"
               "", s->new_socket, inet_ntoa(s->address.sin_addr), ntohs
               (s->address.sin_port));

        // send new connection greeting message
        if (send(s->new_socket, s->message, strlen(s->message), 0) != strlen(s->message))
            perror("send");



        printf("\nWelcome message sent successfully.\n");

    }

    //else its some IO operation on some other socket
    for (int i = 0; i < sizeof(s->client_socket) / sizeof(s->client_socket[0]); i++) {
        s->sd = s->client_socket[i];

        if (FD_ISSET(s->sd, &s->readfds)) {
            //Check if it was for closing, and  read the incoming message
            if ((s->valread = read(s->sd, s->buffer, BUFFER_SIZE)) == 0) {
                //Somebody disconnected , get their details and print
                getpeername(s->sd, (struct sockaddr *) &s->address, (socklen_t *) &s->addrlen);
                printf("Host disconnected , ip %s , port %d \n",
                       inet_ntoa(s->address.sin_addr), ntohs(s->address.sin_port));

                //Close the socket and mark as 0 in list for reuse
                close(s->sd);
                s->client_socket[i] = 0;
            } else {

                //set the string terminating NULL byte on the end of the data read
                s->buffer[s->valread] = '\0';
                printf("%s", s->buffer);

                for (int j = 0; j < (sizeof(s->client_socket) / sizeof(s->client_socket[0])); ++j)
                    send(s->client_socket[j], s->buffer, strlen(s->buffer), 0);
            }
        }

    }

    return 0;
}

/**
 * Create master, set up connections, bind to localhost
 * @param s the server struct
 * @return 0 on success
 */
int setup_server(Server *s) {
    // initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < s->max_clients; i++)
        s->client_socket[i] = 0;

    // create a master socket
    if ((s->master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections
    if(setsockopt(s->master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&s->opt, sizeof(s->opt)) < 0 ) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = INADDR_ANY;
    s->address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(s->master_socket, (struct sockaddr *)&s->address, sizeof(s->address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(s->master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    s->addrlen = sizeof(s->address);
    printf("Waiting for connections... \n");

    return 0;
}