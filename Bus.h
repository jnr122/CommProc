//
// Created by Jonah Rubin on 2020-06-22.
//
#include <stdio.h>

#ifndef COMMPROTOCOL_BUS_H
#define COMMPROTOCOL_BUS_H

typedef struct {
    int const numProcs;
    int const procIDs[3];
    char const filepath[32];
    int rem_reads;

} Bus;

int receive(char *data, Bus b);

int transmit(char *data, Bus b);

#endif //COMMPROTOCOL_BUS_H
