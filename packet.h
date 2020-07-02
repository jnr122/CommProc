//
// Created by Jonah Rubin on 2020-06-29.
//

#ifndef COMMPROTOCOL_PACKET_H
#define COMMPROTOCOL_PACKET_H

#include <stdint.h>
#define FRAME_SIZE 5
#define TRUE 1
#define FALSE 0

typedef struct Packet {
    int head, done, corrupt;
    char frame[FRAME_SIZE];

} Packet;

int process(Packet *p, const char *c);
int disp_packet(const Packet *p);

#endif //COMMPROTOCOL_PACKET_H
