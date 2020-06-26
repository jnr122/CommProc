//
// Created by Jonah Rubin on 2020-06-26.
//

#ifndef COMMPROTOCOL_CLIENT_H
#define COMMPROTOCOL_CLIENT_H

typedef struct Client {
    int status, sock, adrlen, valread;
    struct addrinfo hints;
    struct addrinfo *servinfo;   //will point to the results

    fd_set read_flags, write_flags; // the flag sets to be used
    struct timeval waitd;          // the max wait time for an event
    int sel;

} Client;

int setup_client(Client *c);
int run(Client *c);
int set_nonblock(int socket);

#endif //COMMPROTOCOL_CLIENT_H
