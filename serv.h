//
// Created by Jonah Rubin on 2020-06-26.
//

#ifndef COMMPROTOCOL_SERV_H
#define COMMPROTOCOL_SERV_H

// declare server attributes
typedef struct Server {
    int opt, master_socket, addrlen, new_socket, client_socket[30],
            max_clients, activity, valread, sd, max_sd;
    char buffer[1025], *message;  //data buffer of 1K
    struct sockaddr_in address;

    // set of socket descriptors
    fd_set readfds;
} Server;

int setup_server(Server *s);
int run(Server *s);

#endif //COMMPROTOCOL_SERV_H
