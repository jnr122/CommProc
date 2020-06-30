//
// Created by Jonah Rubin on 2020-06-29.
//

#ifndef COMMPROTOCOL_PACKET_H
#define COMMPROTOCOL_PACKET_H

#include <stdint.h>
#define FRAME_SIZE 5
#define TRUE 1
#define FALSE 0

union cmdPktFmt {
    uint8_t byte[14];
    struct {
        uint16_t SOP;
        uint16_t CRC;
        uint16_t ID;
        uint16_t LeftDrive;
        uint16_t LeftAux;
        uint16_t RightDrive;
        uint16_t RightAux;
    } cmds;
};

typedef struct Packet {
    int head, done;
    char frame[FRAME_SIZE];

} Packet;

int process(Packet *p, const char *c);
int print_packet(const Packet *p);

#endif //COMMPROTOCOL_PACKET_H
