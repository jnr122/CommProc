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

#define PORT "8888"

struct sockaddr name;

void set_nonblock(int socket) {
    int flags;
    flags = fcntl(socket,F_GETFL,0);
    assert(flags != -1);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int agrc, char** argv) {
    int status, sock, adrlen;

    struct addrinfo hints;
    struct addrinfo *servinfo;  //will point to the results

    fd_set read_flags,write_flags; // the flag sets to be used
    struct timeval waitd;          // the max wait time for an event
    int sel;                      // holds return value for select();

    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; //tcp
    hints.ai_flags = AI_PASSIVE;     //use local-host address

    //get server info, put into servinfo
    if ((status = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    //make socket
    sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sock < 0) {
        printf("\nserver socket failure %m", errno);
        exit(1);
    }

    if(connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        printf("\nclient connection failure %m", errno);
        exit(1);
    }

    set_nonblock(sock);

    char out[255];
    char in[255];

    int numRead;
    int numSent;

    while(1) {

        waitd.tv_sec = 10;
        FD_ZERO(&read_flags);
        FD_ZERO(&write_flags);
        FD_SET(sock, &read_flags);
        FD_SET(STDIN_FILENO, &read_flags);

        if(strlen(out) != 0)
            FD_SET(sock, &write_flags);

        sel = select(sock+1, &read_flags, &write_flags, (fd_set*)0, &waitd);

        if(sel < 0)
            continue;

        //socket ready for reading
        if(FD_ISSET(sock, &read_flags)) {

            //clear set
            FD_CLR(sock, &read_flags);

            memset(&in, 0, 255);

            numRead = recv(sock, in, 255, 0);
            if(numRead <= 0) {
                printf("\nClosing socket");
                close(sock);
                break;
            }
            else if(in[0] != '\0')
                printf("%s",in);

        }   //end if ready for read

        //if stdin is ready to be read
        if(FD_ISSET(STDIN_FILENO, &read_flags))
            fgets(out, 255, stdin);


        //socket ready for writing
        if(FD_ISSET(sock, &write_flags)) {
            //printf("\nSocket ready for write");
            FD_CLR(sock, &write_flags);
            send(sock, out, 255, 0);
            memset(&out, 0, 255);
        }   //end if
    }   //end while

    return 0;
}