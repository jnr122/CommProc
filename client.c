#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include <zconf.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <assert.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include "client.h"

#define PORT "8888"
#define TRUE 1
#define BUFFER_SIZE 255
#define FALSE 0

// initialize, set up and run
int main(int agrc, char** argv) {

    Client c;

    setup_client(&c);
    run(&c);

    return 0;
}

/**
 * Make reads non blocking
 * @param socket to adjust
 * @return 0 on success
 */
int set_nonblock(int socket) {
    int flags;
    flags = fcntl(socket,F_GETFL,0);
    assert(flags != -1);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    return 0;
}

/**
 * Connect to server
 * @param c client
 * @return 0 on success
 */
int setup_client(Client *c) {
    memset(&c->hints, 0, sizeof c->hints); //make sure the struct is empty
    c->hints.ai_family = AF_INET;
    c->hints.ai_socktype = SOCK_STREAM; //tcp
    c->hints.ai_flags = AI_PASSIVE;     //use local-host address

    // get server info, put into servinfo
    if ((c->status = getaddrinfo("127.0.0.1", PORT, &c->hints, &c->servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(c->status));
        exit(1);
    }

    // make socket
    c->sock = socket(c->servinfo->ai_family, c->servinfo->ai_socktype, c->servinfo->ai_protocol);
    if (c->sock < 0) {
        printf("\nserver socket failure %m", errno);
        exit(1);
    }

    if(connect(c->sock, c->servinfo->ai_addr, c->servinfo->ai_addrlen) < 0) {
        printf("\nclient connection failure %m", errno);
        exit(1);
    }

    set_nonblock(c->sock);
    return 0;
}

/**
 * Listen for input/ ouput
 * @param c client
 * @return 0 on success
 */
int run(Client *c) {
    char out[BUFFER_SIZE];
    char in[BUFFER_SIZE];

    while(TRUE) {

        c->waitd.tv_sec = 10;
        FD_ZERO(&c->read_flags);
        FD_ZERO(&c->write_flags);
        FD_SET(c->sock, &c->read_flags);
        FD_SET(STDIN_FILENO, &c->read_flags);

        if(strlen(out) != 0)
            FD_SET(c->sock, &c->write_flags);

        c->sel = select(c->sock+1, &c->read_flags, &c->write_flags, (fd_set*)0, &c->waitd);

        if(c->sel < 0)
            continue;

        //socket ready for reading
        if(FD_ISSET(c->sock, &c->read_flags)) {

            //clear set
            FD_CLR(c->sock, &c->read_flags);

            // in
            memset(&in, 0, BUFFER_SIZE);

            // in
            c->valread = recv(c->sock, in, BUFFER_SIZE, 0);
            if(c->valread <= 0) {
                printf("\nClosing socket");
                close(c->sock);
                break;
            }

                // in in
            else if(in[0] != '\0')
                printf("%s",in);

        } //end if ready for read

        // if stdin is ready to be read
        if(FD_ISSET(STDIN_FILENO, &c->read_flags))
            fgets(out, BUFFER_SIZE, stdin);

        // socket ready for writing
        if(FD_ISSET(c->sock, &c->write_flags)) {
            //printf("\nSocket ready for write");
            FD_CLR(c->sock, &c->write_flags);
            send(c->sock, out, BUFFER_SIZE, 0);
            memset(&out, 0, BUFFER_SIZE);
        }
    }   //end while

    return 0;
}